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

void StandardRenderer::renderFrame(GeomInstance* root)
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

// geomnodes are the children of a transformnode or a geomnode
void StandardRenderer::processInstance(GeomInstance* instance)
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

void StandardRenderer::traverseInstance(GeomInstance* instance)
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

void StandardRenderer::processRenderQueue()
{
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

	this->renderQueue.clear();
}
