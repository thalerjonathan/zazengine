/*
 * DRRenderer.cpp
 *
 *  Created on: Jan 16, 2011
 *      Author: jonathan
 */

#include <GL/glew.h>
#include "SDL/SDL.h"

#include "DRRenderer.h"

#include <iostream>
#include <algorithm>
#include <assert.h>

using namespace std;

DRRenderer::DRRenderer(Camera& camera, std::string& skyBoxFolder)
	: Renderer(camera, skyBoxFolder)
{
}

DRRenderer::~DRRenderer()
{
}

void DRRenderer::renderFrame(GeomInstance* root)
{
}

bool DRRenderer::initialize()
{
	return true;
}
