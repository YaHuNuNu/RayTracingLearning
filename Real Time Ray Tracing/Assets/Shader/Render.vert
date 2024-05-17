#version 330 core
layout(location = 0) in vec3 position;
uniform mat4 inverse_VP;
//近平面上各点的世界坐标，用于生成射线
out vec3 worldPos;
out vec2 texCoord;
void main(){
	//通过屏幕坐标求世界空间下的近平面上对应的点坐标
	texCoord = (position.xy + 1) / 2.0;
	vec4 nearPlanePos = inverse_VP * vec4(position.xy, -1, 1);
	worldPos = nearPlanePos.xyz / nearPlanePos.w;
	gl_Position = position.xyzz;
}