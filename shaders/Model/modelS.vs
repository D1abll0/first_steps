#version 150 core

in vec3 pos;
in vec3 norm;
in vec2 tex;
in vec3 tang;
in vec3 bitang;
in ivec4 boneIds; 
in vec4 weights;
	
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float size = 1.0;
uniform float uv = 1;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
	
void main()
{
    TexCoords = tex * uv;
	FragPos = vec3(model * vec4(pos, 1.0));
	Normal = mat3(transpose(inverse(model))) * norm;  
	
    gl_Position =  projection * view * model * vec4(pos*size, 1.0);
	//gl_Position =  projection * view * model * vec4(pos.x, pos.y*size, pos.z, 1.0);	

	
}