/*
 * DRRenderer.h
 *
 *  Created on: Jan 16, 2011
 *      Author: jonathan
 */

#ifndef DRRENDERER_H_
#define DRRENDERER_H_

#include "Renderer.h"

class DRRenderer : public Renderer
{
 public:
	DRRenderer(Camera& camera, std::string&);
	virtual ~DRRenderer();

	virtual bool initialize();

	// renders this list of geominstances which must be in front-to-back order
	void renderFrame(GeomInstance*);

 private:

};

#endif /* DRRENDERER_H_ */
