#include <GL/glew.h>
#include "SDL/SDL.h"

#include "StandardRenderer.h"

#include <iostream>
#include <algorithm>
#include <assert.h>

using namespace std;

bool geomInstanceDistCmp(GeomInstance* a, GeomInstance* b)
{
	return a->distance < b->distance;
}

StandardRenderer::StandardRenderer(Camera& camera, std::string& skyBoxFolder)
	: Renderer(camera, skyBoxFolder)
{
}

StandardRenderer::~StandardRenderer()
{
}

bool
StandardRenderer::initialize()
{
	return true;
}

bool
StandardRenderer::shutdown()
{
	return true;
}

bool
StandardRenderer::renderFrame(GeomInstance* root)
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
	this->processRenderQueue( true );

	SDL_GL_SwapBuffers();

	this->frame++;

	return true;
}
