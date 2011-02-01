/*
 * DRRenderer.h
 *
 *  Created on: Jan 16, 2011
 *      Author: jonathan
 */

#ifndef DRRENDERER_H_
#define DRRENDERER_H_

#define MRT_COUNT 4

#include "Renderer.h"

class DRRenderer : public Renderer
{
 public:
	DRRenderer(Camera& camera, std::string&);
	virtual ~DRRenderer();

	virtual bool initialize();
	virtual bool shutdown();

	// renders this list of geominstances which must be in front-to-back order
	bool renderFrame(GeomInstance*);

 private:
	GLuint m_frameBuffer;
	GLuint m_mrt[ MRT_COUNT ];

	UniformBlock* m_transformBlock;
	Program* m_geomStageProg;
	Shader* m_vertShaderGeomStage;
	Shader* m_fragShaderGeomStage;

	bool initFBO();

	bool initGeomStage();
	bool initLightingStage();

};

#endif /* DRRENDERER_H_ */
