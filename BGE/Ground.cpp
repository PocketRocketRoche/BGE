#include "Ground.h"
#include "Content.h"
#include <gtc/matrix_inverse.hpp>

using namespace BGE;

Ground::Ground(void):GameComponent()
{
	width = 500;
	height = 500;
	// Diffuse will come from the texture
	ambient = glm::vec3(0.2f, 0.2, 0.2f);
	specular = glm::vec3(0,0,0);
}


Ground::~Ground(void)
{
}

bool Ground::Initialise()
{
	const float twidth = 50;
    const float theight = 50;

	if (!GameComponent::Initialise()) {
		return false;
	}


	vertices.push_back(glm::vec3(width, 0, -height));
	normals.push_back(GameComponent::basisUp);
	texels.push_back(glm::vec2(twidth, 0));

	vertices.push_back(glm::vec3(-width, 0, -height));
	normals.push_back(GameComponent::basisUp);
	texels.push_back(glm::vec2(0, 0));

	vertices.push_back(glm::vec3(width, 0, height));
	normals.push_back(GameComponent::basisUp);
	texels.push_back(glm::vec2(twidth, theight));
    
	vertices.push_back(glm::vec3(width, 0, height));
	normals.push_back(GameComponent::basisUp);
	texels.push_back(glm::vec2(twidth, theight));
	
	vertices.push_back(glm::vec3(-width, 0, -height));
	normals.push_back(GameComponent::basisUp);
	texels.push_back(glm::vec2(0,0));
	
	vertices.push_back(glm::vec3(-width, 0, height));
	normals.push_back(GameComponent::basisUp);
	texels.push_back(glm::vec2(0, theight)); 
    
	programID = Content::LoadShaderPair("standard_texture");

	textureID = Content::LoadTexture("ground");

	glGenBuffers(1, &vertexbuffer); 
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), & vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &texelbuffer); 
	glBindBuffer(GL_ARRAY_BUFFER, texelbuffer);
	glBufferData(GL_ARRAY_BUFFER, texels.size() * sizeof(glm::vec2), & texels[0], GL_STATIC_DRAW);

	mID = glGetUniformLocation(programID,"m");
	vID = glGetUniformLocation(programID,"v");
	pID = glGetUniformLocation(programID,"p");
	nID = glGetUniformLocation(programID,"n");
	ambientID = glGetUniformLocation(programID,"ambient");
	specularID = glGetUniformLocation(programID,"specular");
	
	textureSampler  = glGetUniformLocation(programID, "myTextureSampler");

	return true;
}

void Ground::Draw()
{
	glUseProgram(programID);

	glUniformMatrix4fv(mID, 1, GL_FALSE, & world[0][0]);
	glUniformMatrix4fv(vID, 1, GL_FALSE, & Game::Instance()->GetCamera()->GetView()[0][0]);
	glUniformMatrix4fv(pID, 1, GL_FALSE, & Game::Instance()->GetCamera()->GetProjection()[0][0]);
	glUniform3f(specularID, specular.r, specular.g, specular.b);
	glUniform3f(ambientID, ambient.r, ambient.g, ambient.b);


	glm::mat4 MV = Game::Instance()->GetCamera()->GetView() * world;
	glm::mat3 gl_NormalMatrix = glm::inverseTranspose(glm::mat3(MV));
	glUniformMatrix3fv(nID, 1, GL_FALSE, & gl_NormalMatrix[0][0]);
	
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(textureSampler, 0);

	// 0th  attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 1st attribute buffer : normals
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_TRUE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 2nd attribute buffer : texels
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, texelbuffer);
	glVertexAttribPointer(
		2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 6); 
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	
	// Dont forget to call the superclass draw so that OpenGL Swap Buffers can be called
	GameComponent::Draw();
}
void Ground::Cleanup()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &texelbuffer);
	glDeleteTextures(1, &textureID);
	glDeleteProgram(programID);
}