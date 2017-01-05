#version 330 core


out vec4 color;
in vec3 pos;

uniform float time;
uniform vec4 uniColor;

void main(){
	color = vec4(sin(time),0.2,uniColor.b,uniColor.a);
}