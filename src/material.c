#include "material.h"
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

////////////////////////////////////////////////////////////////////////////////

static void texmgr_free(Texture* tex)
{
	if (tex->glTexture) glDeleteTextures(1, &tex->glTexture);
	if (tex->data)      free(tex->data);
}
static bool texmgr_load(Texture* tex, const char* fullPath)
{
	int width, height;
	unsigned char* image = SOIL_load_image(fullPath, &width, &height, 0, SOIL_LOAD_RGBA);
	if (!image) {
		LOG("load_image() failed: '%s'\n", fullPath);
		return false;
	}

	tex->data = NULL;
	glGenTextures(1, &tex->glTexture);
	glBindTexture(GL_TEXTURE_2D, tex->glTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	SOIL_free_image_data(image);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

TexManager* tex_manager_create(int maxCount) {
	return (TexManager*)res_manager_create(maxCount, sizeof(Texture), 
		(ResMgr_LoadFunc)texmgr_load, (ResMgr_FreeFunc)texmgr_free);
}
Texture* tex_manager_data(TexManager* t) {
	return (Texture*)res_manager_data(&t->rm);
}
Texture* texture_load(TexManager* t, const char* texturePath) {
	return (Texture*)resource_load(&t->rm, texturePath);
}
void texture_free(Texture* tex) {
	resource_free(&tex->res);
}
void tex_manager_clean_unused(TexManager* t) {
	res_manager_clean_unused(&t->rm);
}
void tex_manager_destroy_all_items(TexManager* t) {
	res_manager_destroy_all_items(&t->rm);
}
void tex_manager_destroy(TexManager* t) {
	res_manager_destroy(&t->rm);
}
////////////////////////////////////////////////////////////////////////////////

static const vec4 WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };

Material material_create(Shader* shader, Texture* texture)
{
	Material m;
	m.color   = WHITE;
	m.shader  = shader;
	m.texture = texture;
	return m;
}
Material material_from_file(ShaderManager* smgr, const char* shaderName, 
                            TexManager*    tmgr, const char* texturePath)
{
	Material m;
	m.color   = WHITE;
	m.shader  = shader_load(smgr, shaderName);
	m.texture = texture_load(tmgr, texturePath);
	return m;
}

void material_destroy(Material* m)
{
	if (m->shader)  shader_free(m->shader), m->shader  = NULL;
	if (m->texture) texture_free(m->texture),      m->texture = NULL;
}

void material_move(Material* dst, Material* src)
{
	Shader*  s = dst->shader;
	Texture* t = dst->texture;
	dst->shader  = src->shader;
	dst->texture = src->texture;
	src->shader  = s;
	src->texture = t;
}


////////////////////////////////////////////////////////////////////////////////