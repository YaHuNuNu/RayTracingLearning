#version 330 core
//近平面上各点的世界坐标，用于生成射线
in vec3 worldPos;
in vec2 texCoord;

//顶点和AABB Tree数据
uniform samplerBuffer TTB;
uniform samplerBuffer NTB;
uniform sampler2D hdrTexture;
//帧缓冲
//uniform sampler2D texColor;
uniform uint frameCounter;
uniform vec2 WindowSize;
//摄像机的世界坐标
uniform vec3 cameraPos;

const float MAX = 1e7;
const float MIN = -1e7;
const float PI =  3.1415926f;
vec3 albedo = vec3(0.1f, 0.6f, 0.8f);
float roughness = 0.8f;
float metallic = 0.2f;

struct Ray{
	vec3 origin;
	vec3 direction;
};

struct Triangle{
	vec3 vertex[3];
	vec3 normal[3];
};

struct AABB{
	//对vertices中的三角形的索引，表示该包围盒所包含的三角形
	int l, r;
	//子节点索引，-1表示无子节点
	int LChild, RChild;
	vec3 Bmin, Bmax;
};

struct HitInfo{
	bool hit;
    float t;
	vec3 hitPos;
	vec3 normal;
};
//存储击中点信息
HitInfo INFO;
HitInfo FirstInfo;

Triangle GetTriangle(int index){
	index *= 6;
	Triangle t;
	t.vertex[0] = texelFetch(TTB, index + 0).xyz;
	t.vertex[1] = texelFetch(TTB, index + 1).xyz;
	t.vertex[2] = texelFetch(TTB, index + 2).xyz;
	t.normal[0] = texelFetch(TTB, index + 3).xyz;
	t.normal[1] = texelFetch(TTB, index + 4).xyz;
	t.normal[2] = texelFetch(TTB, index + 5).xyz;
	return t;
}

AABB GetAABB(int index){
	index *= 4;
	AABB t;
	t.Bmin = texelFetch(NTB, index).xyz;
	t.Bmax = texelFetch(NTB, index + 1).xyz;
	vec2 child = texelFetch(NTB, index + 2).xy;
	t.LChild = int(child.x);
	t.RChild = int(child.y);
	vec2 l_r = texelFetch(NTB, index + 3).xy;
	t.l = int(l_r.x);
	t.r = int(l_r.y);
	return t;
}

//检测射线是否击中三角形(通过索引)。使用Trumbore算法
void TriangleIntersect(Ray ray, int index){
	Triangle triangle = GetTriangle(index);
	vec3 S = ray.origin - triangle.vertex[0];
	vec3 E1 = triangle.vertex[1] - triangle.vertex[0];
	vec3 E2 = triangle.vertex[2] - triangle.vertex[0];

	vec3 DxE2 = cross(ray.direction, E2);
	vec3 SxE1 = cross(S, E1);

	float parameter = 1.0f / dot(DxE2, E1);

	float t = parameter * dot(SxE1, E2);
	float u = parameter * dot(DxE2, S);
	float v = parameter * dot(SxE1, ray.direction);
	
	if (t > 0.0f && u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f) {
		if(t < INFO.t){
			INFO.hit = true;
			INFO.t = t;
			float a = 1 - u - v;
			INFO.normal = a * triangle.normal[0] + u * triangle.normal[1] + v * triangle.normal[2];
			INFO.hitPos = a * triangle.vertex[0] + u * triangle.vertex[1] + v * triangle.vertex[2];
		}
	}
}

//检测射线是否击中AABB(通过索引)，返回-1为未击中
float AABBIntersect(Ray ray, int index){
	AABB box = GetAABB(index);
	//T0记录射入点，T1记录射出点
	float T0 = MIN, T1 = MAX;
	vec3 deltaA = (box.Bmin - ray.origin);
	vec3 deltaB = (box.Bmax - ray.origin);

	//X
	if(ray.direction.x == 0 && ((deltaA.x > 0 || deltaB.x < 0))) //排除平行于轴的情况
			return -1;
	else{
		float a1 = deltaA.x / ray.direction.x;
		float a2 = deltaB.x / ray.direction.x;
		//排除射线正方向无法于BOX相交的情况
		if(a1 < 0 && a2 < 0)
			return -1;
		//T0：a1,a2之间最小的为入射长度,T0为最大的入射长度；T1与T0相反
		T0 = max(T0, min(a1, a2));
		T1 = min(T1, max(a1, a2));
	}

	//Y
	if(ray.direction.y == 0 && ((deltaA.y > 0 || deltaB.y < 0)))
			return -1;
	else{
		float a1 = deltaA.y / ray.direction.y;
		float a2 = deltaB.y / ray.direction.y;
		if(a1 < 0 && a2 < 0)
			return -1;
		T0 = max(T0, min(a1, a2));
		T1 = min(T1, max(a1, a2));
	}

	//Z
	if(ray.direction.z == 0 && ((deltaA.z > 0 || deltaB.z < 0)))
			return -1;
	else{
		float a1 = deltaA.z / ray.direction.z;
		float a2 = deltaB.z / ray.direction.z;
		if(a1 < 0 && a2 < 0)
			return -1;
		T0 = max(T0, min(a1, a2));
		T1 = min(T1, max(a1, a2));
	}

	return T1 > T0 ? T1 : -1;
}

//对Node节点中的所有三角形求交
void NodeIntersect(Ray ray, AABB node){
	for(int i = node.l; i <= node.r; i++)
		TriangleIntersect(ray, i);
}

//遍历AABB树
void Search(Ray ray){
	//初始化INFO
	INFO.t = MAX;
	INFO.hit = false;
	//根节点处理
	if(AABBIntersect(ray, 0) == -1)
		return;
	//初始化栈
	int stack[1000];
	int p = -1;
	stack[++p] = 0;

	while(p > -1){
		int index = stack[p--];
		AABB node = GetAABB(index);

		//叶节点处理
		if(node.LChild < 0){
			//判断产生交点后直接return原则上可以得到最近的交点，但实际出现了较多误差，暂不清楚原因
			NodeIntersect(ray, node);
			continue;
		}

		//计算子节点的相交，入栈
		int l = node.LChild;
		int r = node.RChild;
		float t1 = AABBIntersect(ray, l);
		float t2 = AABBIntersect(ray, r);
		
		if(t1 > 0 && t2 > 0){
			if(t1 > t2){
				//近的后入栈，以便于先处理
				stack[++p] = l;
				stack[++p] = r;
			}
			else{
				stack[++p] = r;
				stack[++p] = l;
			}
		}
		else if(t1 > 0)
			stack[++p] = l;
		else if(t2 > 0)
			stack[++p] = r;
	}
}

uint GetSeed(uint FC){
return uint(
    uint(texCoord.x * WindowSize[0])  * uint(1973) + 
    uint(texCoord.y * WindowSize[1]) * uint(9277) + 
    uint(FC) * uint(26699)) | uint(1);
}

uint wang_hash(uint seed) {
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}
 
float rand(uint FC) {
    return float(wang_hash(GetSeed(FC)) / 4294967296.0);
}

// 半球均匀采样
vec3 UniformSampleHemisphere() {
    float z = rand(frameCounter);
    float r = max(0, sqrt(1.0 - z*z));
    float phi = 2.0 * PI * z;
    vec3 L = vec3(r * cos(phi), r * sin(phi), z);
	return L;
}

//重要性采样
vec3 ImportanceSampleGGX(vec3 V, float a){
	vec2 Xi = vec2(rand(frameCounter), rand(frameCounter + uint(10)));
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	vec3 H;//半程向量
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	vec3 L = 2.0f * dot(H, V) * H - V;
	return L;
}

//使用TBN矩阵从切线空间转到世界空间
vec3 TangentToWorldSpace(vec3 L, vec3 N) {
    	//考虑到法向量N与(0, 0, 1)重合时
	vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);

	vec3 sampleVec = tangent * L.x + bitangent * L.y + N * L.z;
	return normalize(sampleVec);
}


vec3 fresnelSchlick(float HdotV, vec3 F0)
{
	return F0 + (1.0f - F0) * pow(HdotV, 5.0f);
}

float DistributionGGX(float NdotH, float a)
{
	float a2 = a * a;
	float temp = NdotH * NdotH * (a2 - 1) + 1;

	return a2 / (PI * temp * temp);
}

float GeometrySchlickGGX(float NdotX, float k)
{
	return NdotX / (NdotX * (1.0f - k) + k);
}

float GeometrySmith(float NdotV, float NdotL, float a)
{
	//直接光照时，k = (a + 1)^2 / 8.0
	//float k = a * a / 2.0;
	float k = (a + 1) * (a + 1) / 8.0f;
	return GeometrySchlickGGX(NdotV, k) * GeometrySchlickGGX(NdotL, k);
}

vec3 BRDF(vec3 V, vec3 L, vec3 H)
{
	float HdotV = dot(H, V);
	float NdotV = dot(INFO.normal, V);
	float NdotL = dot(INFO.normal, L);
	float NdotH = dot(INFO.normal, H);
	float a = roughness * roughness;

	vec3 P = albedo * (1.0f - metallic);
	vec3 F0 = mix(vec3(0.04f), albedo, metallic);

	vec3 F = fresnelSchlick(HdotV, F0);
	float G = GeometrySmith(NdotV, NdotL, a);
	float D = DistributionGGX(NdotH, a);

	vec3 specular = F * G * D / (4.0f * NdotV * NdotL);
	vec3 diffuse = (1.0f - F) * P / PI;

	return diffuse + specular; 
}

vec2 GetSphericalTextureUV(vec3 dir) {
    vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y));
    uv /= vec2(2.0f * PI, PI);
    uv += 0.5f;
    uv.y = 1.0f - uv.y;
    return uv;
}

vec3 SampleHdrTexture(vec3 dir) {
    vec2 uv = GetSphericalTextureUV(normalize(dir));
    vec3 color = texture2D(hdrTexture, uv).rgb;
    //color /= (color + vec3(1));
    return color;
}

vec3 RayTracing(Ray ray, int bounce){
		//初始化初次击中点
		FirstInfo.hit = false;

		vec3 stack[8];
		int p = -1;
		vec3 Color = vec3(0);

		while(bounce > 0){
		bounce--;
		Search(ray);
		
		if(!FirstInfo.hit)
			FirstInfo = INFO;

		if(!INFO.hit)
		{
			Color = SampleHdrTexture(ray.direction);
			Color /= (Color + vec3(1));
			break;
		}
		float a = roughness * roughness;
		vec3 V = - ray.direction;
		vec3 L = TangentToWorldSpace(UniformSampleHemisphere(), INFO.normal);
		//vec3 L = TangentToWorldSpace(ImportanceSampleGGX(V, a), INFO.normal);
		vec3 H = normalize(L + V);
		float NdotH = dot(INFO.normal, H);
		float NdotL = dot(INFO.normal, L);
		float HdotL = dot(H, L);
		vec3 Fr = BRDF(V, L, H);
		float D = DistributionGGX(NdotH, a);
		float pdf = D *  NdotH / (4.0f * HdotL);
		stack[++p] = Fr * NdotL / pdf;
		Ray newRay;
		newRay.origin = INFO.hitPos + L * 0.00001f;
		newRay.direction = L;
		ray = newRay;
		}
		
		while(p > -1){
			vec3 val = stack[p--];
			Color = Color * val;
		}

		return Color /= (Color + vec3(1));
}

void main()
{
	Ray ray;
	ray.origin = cameraPos;
	ray.direction = normalize(worldPos - cameraPos);
	vec3 color = RayTracing(ray, 3);
	if(!FirstInfo.hit){
	FirstInfo.hitPos = vec3(MAX);
	FirstInfo.normal = vec3(0, 1, 0);
	}
	gl_FragData[0] = vec4(color, 1);
	gl_FragData[1] = vec4(FirstInfo.normal, 1);
	gl_FragData[2] = vec4(FirstInfo.hitPos, 1);
} 

