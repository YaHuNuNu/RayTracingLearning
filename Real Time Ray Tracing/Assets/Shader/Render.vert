#version 330 core
layout(location = 0) in vec3 position;
uniform mat4 inverse_VP;
//��ƽ���ϸ�����������꣬������������
out vec3 worldPos;
out vec2 texCoord;
void main(){
	//ͨ����Ļ����������ռ��µĽ�ƽ���϶�Ӧ�ĵ�����
	texCoord = (position.xy + 1) / 2.0;
	vec4 nearPlanePos = inverse_VP * vec4(position.xy, -1, 1);
	worldPos = nearPlanePos.xyz / nearPlanePos.w;
	gl_Position = position.xyzz;
}