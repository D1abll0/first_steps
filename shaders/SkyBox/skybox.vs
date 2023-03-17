#version 150 core

in vec3 aPos;

out vec3 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoords = aPos;
	vec4 pos = projection * view * model * vec4(aPos, 1);
	gl_Position = pos.xyww;
}