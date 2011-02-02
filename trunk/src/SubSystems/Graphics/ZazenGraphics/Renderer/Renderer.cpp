#include <GL/glew.h>
#include <SDL/SDL.h>
#include <GL/glut.h>
#include <GL/glu.h>

#include "Renderer.h"

#include <algorithm>
#include <iostream>

using namespace std;

Renderer::Renderer( Camera& camera, std::string& skyBoxFolder )
	: camera( camera )
{
	this->renderedFaces = 0;
	this->renderedInstances = 0;

	this->culledInstances = 0;
	this->occludedInstances = 0;

	this->frame = 1;
	this->skyBox = 0;
	
	if (skyBoxFolder != "")
		this->skyBox = new GeomSkyBox(this->camera, skyBoxFolder);

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glCullFace(GL_FRONT);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	GLfloat LightAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPosition[] = { 0.0f, 60.0f, 0.0f, 1.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// Setup The Ambient Light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// Setup The Diffuse Light
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// Position The Light
	glEnable(GL_LIGHT1);								// Enable Light One
}

Renderer::~Renderer()
{

}

void
Renderer::printInfo()
{
	cout << "--------------------------------------- SCENE-INFO ---------------------------------------" << endl;
	cout << "Rendered Faces:          " << this->renderedFaces << endl << endl;
	cout << "Rendered Instances:      " << this->renderedInstances << endl;
	cout << "Culled Instances:        " << this->culledInstances << endl;
	cout << "Occluded Instances:      " << this->occludedInstances << endl;
	cout << "------------------------------------------------------------------------------------------" << endl;
}

// geomnodes are the children of a transformnode or a geomnode
/*
void
Renderer::processInstance( Instance* instance )
{
	instance->recalculateDistance();

	bool backupFlag = this->parentIntersectingFrustum;

	if ( instance->parent == 0 || this->parentIntersectingFrustum )
	{
		Vector bbMinWorld(instance->geom->getBBMin());
		Vector bbMaxWorld(instance->geom->getBBMax());

		instance->transform.transform(bbMinWorld);
		instance->transform.transform(bbMaxWorld);

		CullResult result = this->camera.cullBB(bbMinWorld, bbMaxWorld);
		if (result == OUTSIDE)
			return;

		if (result == INTERSECTING)
			this->parentIntersectingFrustum = true;
		else
			this->parentIntersectingFrustum = false;
	}

	this->traverseInstance( instance );

	this->parentIntersectingFrustum = backupFlag;
}

void
Renderer::traverseInstance( Instance* instance )
{
	// not a leaf-node
	if ( instance->children.size() > 0 )
	{
		instance->visible = false;

		// also sort children in front-to-back
		sort( instance->children.begin(), instance->children.end(), Renderer::geomInstanceDistCmp );

		for (unsigned int i = 0; i < instance->children.size(); i++)
			this->processInstance(instance->children[i]);

	// is a leaf-node => render
	}
	else
	{
		this->renderQueue.push_back(instance);
	}
}

void
Renderer::processRenderQueue()
{
	if (this->skyBox)
		this->skyBox->render();

	list<Instance*>::iterator iter = this->renderQueue.begin();
	while ( iter != this->renderQueue.end() )
	{
		Instance* instance = *iter++;

		glLoadMatrixf( instance->transform.data );
		instance->geom->render();

		this->renderedFaces += instance->geom->getFaceCount();
		this->renderedInstances++;
	}

	this->renderQueue.clear();
}
*/

void
Renderer::renderGeom( GeomType* geom )
{
	if ( geom->children.size() )
	{
		for ( unsigned int i = 0; i < geom->children.size(); i++ )
		{
			GeomType* child = geom->children[ i ];

			glPushMatrix();
			glLoadMatrixf( child->model_transf.data );

			child->render();

			glPopMatrix();
		}
	}
	else
	{
		geom->render();
	}
}
