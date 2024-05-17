#version 330 core
layout(location = 0) in vec3 position;
out vec2 texCoord;
void main(){
	texCoord = (position.xy + 1) / 2.0;
	gl_Position = position.xyzz;
}