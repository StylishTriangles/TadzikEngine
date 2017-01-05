#version 330 core

layout (location = 0) in vec3 position;

uniform float time;
uniform mat4 mx;

out vec3 pos;

void main()
{
	//A*sin(dist(x,y,center)+time)
	float h = 0.2*sin(length(position.xy)+time*1.3);
	gl_Position = mx*vec4(position.x, position.y, h, 1.0);
	pos = gl_Position.xyz;
}