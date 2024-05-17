#version 330 core
uniform sampler2D texColor;
uniform sampler2D texNormal;
uniform sampler2D texPosition;

uniform vec2 WindowSize;

in vec2 texCoord;
out vec4 color;

int kernelRadius = 32;
float m_alpha = 0.2f;
float m_sigmaPlane = 0.1f;
float m_sigmaColor = 0.6f;
float m_sigmaNormal = 0.1f;
float m_sigmaCoord = 32.0f;
float m_colorBoxK = 1.0f;

float SqrDistance(vec3 p1, vec3 p2){
	return dot(p2 - p1, p2 - p1);
}

vec3 Filter(){
	ivec2 start = ivec2(max(vec2(0), gl_FragCoord.xy - vec2(kernelRadius)));
	ivec2 end = ivec2(min(WindowSize - vec2(1), gl_FragCoord.xy + vec2(kernelRadius)));
	
	vec3 centerColor = texture2D(texColor, texCoord).xyz;
	vec3 centerNormal = texture2D(texNormal, texCoord).xyz;
	vec3 centerPos = texture2D(texPosition, texCoord).xyz;

	vec3 finalColor = vec3(0);
	float totalWeight = 0;
	
	for(int i = start.x; i <= end.x; i++){
		for(int j = start.y; j <= end.y; j++){
			vec2 sampleCoord = (vec2(i, j) + vec2(0.5f)) / WindowSize;
			vec3 color = texture2D(texColor, sampleCoord).xyz;
			vec3 normal = texture2D(texNormal, sampleCoord).xyz;
			vec3 pos = texture2D(texPosition, sampleCoord).xyz;
			
			float dPos = SqrDistance(centerPos, pos) / (2.0f * m_sigmaCoord * m_sigmaCoord);
			float dColor = SqrDistance(centerColor, color) / (2.0f * m_sigmaColor * m_sigmaColor);
			float dNormal = acos(clamp(dot(centerNormal, normal), -1, 1));
			dNormal *= dNormal;
            dNormal / (2.0f * m_sigmaNormal * m_sigmaNormal);
			float dPlane = 0.0f;
            if (dPos > 0.0f) {
				dPlane = dot(centerNormal, normalize(pos - centerPos));
            }
			dPlane *= dPlane;
			dPlane /= (2.0f * m_sigmaPlane * m_sigmaPlane);

			float weight = exp(-dPlane - dPos - dColor - dNormal);
			totalWeight += weight;
            finalColor += color * weight;
		}
	}
	
	return finalColor / totalWeight;
}


void main(){
	if(texture2D(texPosition, texCoord).x > 1e6)
		color = texture2D(texColor, texCoord);
	else
		color = vec4(Filter(), 1);
		//color = texture2D(texColor, texCoord);
}