#include <GL/glew.h>
#include "SDL/SDL.h"

#include "SFXRenderer.h"

#include <iostream>
#include <algorithm>
#include <assert.h>

using namespace std;

#define CHECK_FRAMEBUFFER_STATUS() \
{\
 GLenum status; \
 status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
 switch(status) { \
 case GL_FRAMEBUFFER_COMPLETE_EXT: \
   break; \
 case GL_FRAMEBUFFER_UNSUPPORTED_EXT: \
   fprintf(stderr,"framebuffer GL_FRAMEBUFFER_UNSUPPORTED_EXT\n");\
    /* you gotta choose different formats */ \
   assert(0); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: \
   fprintf(stderr,"framebuffer INCOMPLETE_ATTACHMENT\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: \
   fprintf(stderr,"framebuffer FRAMEBUFFER_MISSING_ATTACHMENT\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: \
   fprintf(stderr,"framebuffer FRAMEBUFFER_DIMENSIONS\n");\
   break; \
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: \
   fprintf(stderr,"framebuffer INCOMPLETE_FORMATS\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: \
   fprintf(stderr,"framebuffer INCOMPLETE_DRAW_BUFFER\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: \
   fprintf(stderr,"framebuffer INCOMPLETE_READ_BUFFER\n");\
   break; \
 case GL_FRAMEBUFFER_BINDING_EXT: \
   fprintf(stderr,"framebuffer BINDING_EXT\n");\
   break; \
/*
 case GL_FRAMEBUFFER_STATUS_ERROR_EXT: \
   fprintf(stderr,"framebuffer STATUS_ERROR\n");\
   break; \
*/ \
 default: \
   /* programming error; will fail on all hardware */ \
   assert(0); \
 }\
}

SFXRenderer::SFXRenderer(Camera& camera, std::string& skyBoxFolder)
	: Renderer(camera, skyBoxFolder)
{
	this->createFBO(&this->backgroundTex, &this->backgroundFBO);
}

SFXRenderer::~SFXRenderer()
{
}

void SFXRenderer::renderFrame(GeomInstance* root)
{
	this->renderedFaces = 0;
	this->renderedInstances = 0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadMatrixf(this->camera.modelView.data);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glColor4f(1, 0, 0, 0);
	glBegin(GL_LINES);
		glVertex3f(100, 0, 0);
		glVertex3f(-100, 0, 0);
	glEnd();

	glColor4f(0, 1, 0, 0);
	glBegin(GL_LINES);
		glVertex3f(0, 100, 0);
		glVertex3f(0, -100, 0);
	glEnd();

	glColor4f(0, 0, 1, 0);
	glBegin(GL_LINES);
		glVertex3f(0, 0, 100);
		glVertex3f(0, 0, -100);
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	this->processInstance(root);
	this->processRenderQueue();

	SDL_GL_SwapBuffers();

	this->frame++;
}

void SFXRenderer::processRenderQueue()
{

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->backgroundFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (this->skyBox)
		this->skyBox->render();

	list<GeomInstance*>::iterator iter = this->renderQueue.begin();
	while (iter != this->renderQueue.end()) {
		GeomInstance* instance = *iter++;

		Material* material = instance->geom->material;
		if (material) {
			if (material->isTransparent())
				continue;
			else
				material->activate();
		}

		glLoadMatrixf(instance->transform.data);
		instance->geom->render();

		if (material)
			material->deactivate();

		this->renderedFaces += instance->geom->getFaceCount();
		this->renderedInstances++;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->backgroundTex);

	iter = this->renderQueue.begin();
	while (iter != this->renderQueue.end()) {
		GeomInstance* instance = *iter++;

		Material* material = instance->geom->material;
		if (material) {
			if (material->isTransparent() == false)
				continue;
			else
				material->activate();
		}

		glLoadMatrixf(instance->transform.data);
		instance->geom->render();

		if (material)
			material->deactivate();
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	this->renderQueue.clear();
}

// geomnodes are the children of a transformnode or a geomnode
void SFXRenderer::processInstance(GeomInstance* instance)
{
	instance->recalculateDistance();

	bool backupFlag = this->parentIntersectingFrustum;

	if (instance->parent == 0 || this->parentIntersectingFrustum) {
		Vector bbMinWorld(instance->geom->getBBMin());
		Vector bbMaxWorld(instance->geom->getBBMax());

		instance->transform.transform(bbMinWorld);
		instance->transform.transform(bbMaxWorld);

		CullResult result = this->camera.cullBB(bbMinWorld, bbMaxWorld);
		if (result == OUTSIDE) {
			return;
		}

		if (result == INTERSECTING) {
			this->parentIntersectingFrustum = true;
		} else {
			this->parentIntersectingFrustum = false;
		}
	}

	this->traverseInstance(instance);

	this->parentIntersectingFrustum = backupFlag;
}

void SFXRenderer::traverseInstance(GeomInstance* instance)
{
	// not a leaf-node
	if (instance->children.size() > 0) {
		instance->visible = false;

		// also sort children in front-to-back
		sort(instance->children.begin(), instance->children.end(), Renderer::geomInstanceDistCmp);

		for (unsigned int i = 0; i < instance->children.size(); i++) {
			this->processInstance(instance->children[i]);
		}

	// is a leaf-node => render
	} else {
		this->renderQueue.push_back(instance);
	}
}

void SFXRenderer::createFBO(GLuint* texID, GLuint* fboID)
{
	glGenFramebuffersEXT(1, fboID);
	glGenTextures(1, texID);
	if (glGetError() != GL_NO_ERROR)
		cout << "glGenTextures failed with " << gluErrorString(glGetError()) << endl;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, *fboID);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, *texID);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, this->camera.getWidth(), this->camera.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	if (glGetError() != GL_NO_ERROR)
		cout << "glTexImage2D failed with " << gluErrorString(glGetError()) << endl;

	glClearColor(0, 0, 0, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, *texID, 0);

	CHECK_FRAMEBUFFER_STATUS()

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
