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
	
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

uniform float size;
uniform float uv;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
	
void main()
{
    TexCoords = tex * uv;
	FragPos = vec3(model * vec4(pos, 1.0));
	Normal = mat3(transpose(inverse(model))) * norm;  
	
    vec4 totalPosition = vec4(0.0f);
	for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
	{
		if(boneIds[i] == -1) 
			continue;
		if(boneIds[i] >=MAX_BONES) 
		{
			totalPosition = vec4(pos,1.0f);
			break;
		}
		vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);
		totalPosition += localPosition * weights[i];
		vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
	}

	totalPosition *= vec4(vec3(size), 1.0);
    gl_Position =  projection * view * model * totalPosition;
}