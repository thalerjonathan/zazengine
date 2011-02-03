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

#include "../Material/UniformBlock.h"
#include "../Material/Program.h"
#include "../Material/Shader.h"

class DRRenderer : public Renderer
{
 public:
	DRRenderer(Camera& camera, std::string&);
	virtual ~DRRenderer();

	virtual bool initialize();
	virtual bool shutdown();

	// renders this list of geominstances which must be in front-to-back order
	bool renderFrame( std::list<Instance*>& instances );

 private:
	GLuint m_frameBuffer;
	GLuint m_mrt[ MRT_COUNT ];

	Program* m_geomStageProg;
	Shader* m_vertShaderGeomStage;
	Shader* m_fragShaderGeomStage;

	UniformBlock* m_transformBlock;

	bool initFBO();

	bool initGeomStage();
	bool initLightingStage();

	bool renderGeom( Matrix& transf, GeomType* );

};

#endif /* DRRENDERER_H_ */
