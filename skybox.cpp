#include "skybox.h"

Skybox::Skybox() : 
	name(skyboxPicName), suffix(skyboxPicSuf) {}

Skybox::~Skybox() {
}

void Skybox::init() {
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	int width, height;
	unsigned char* image;
	for (int i = 0; i < 6; i++) {
		string picName = "skybox/" + name + "_" + partName[i] + suffix;
		image = SOIL_load_image(picName.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 
			width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
		);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	shader.attach(GL_VERTEX_SHADER, skyboxVertexShaderPath);
	shader.attach(GL_FRAGMENT_SHADER, skyboxFragmentShaderPath);
	shader.link();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GLfloat), skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Skybox::render() {
	glDepthMask(GL_FALSE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)0);

	shader.begin();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.end();

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDepthMask(GL_TRUE);
}
