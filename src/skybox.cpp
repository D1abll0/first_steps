#include "../include/skybox.h"

SkyBox::SkyBox(Shader& shader, const string& TexPath, float size)
{
	this->TexPath = TexPath;
	this->shader = shader;

	SetSize(size);
	setupMesh();
	textureID = loadCubemap(TexPath.c_str());
	
	shader.use();
	shader.setInt("skybox", 0);
}

void SkyBox::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);
//	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
//	
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SkyBox::Draw()
{
	glDepthFunc(GL_LEQUAL);

	shader.use();
	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}

void SkyBox::SetViewMat(mat4 mat)
{
	view = mat;
}

void SkyBox::SetProjMat(mat4 mat)
{
	projection = mat;
}

void SkyBox::SetSize(float size)
{
	model = scale(mat4(1.0), vec3(size, size, size));
}
