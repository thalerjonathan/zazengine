#include <GL/glew.h>
#include <SDL/SDL.h>
#include <GL/glut.h>
#include <GL/glu.h>

#include "Renderer.h"

#include <algorithm>
#include <iostream>

using namespace std;

Renderer::Renderer( Viewer* camera )
{
	this->m_camera = camera;

	this->frame = 0;
}

Renderer::~Renderer()
{
}
