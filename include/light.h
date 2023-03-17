#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Light
{
	vec3 Position;
	vec3 Direction;
	
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	
};

#endif