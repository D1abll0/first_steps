#version 150 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
//    FragColor = texture(texture_diffuse1, TexCoords);
	vec4 texColor = texture(texture_diffuse1, TexCoords);
	if(texColor.a < 0.1)
		discard;
	FragColor = texColor;
}