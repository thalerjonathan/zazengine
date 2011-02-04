/*
 * DRRenderer.h
 *
 *  Created on: Jan 16, 2011
 *      Author: jonathan
 */

#ifndef DRRENDERER_H_
#define DRRENDERER_H_

// number of rendering targets for deferred renderer
#define MRT_COUNT 4

#include "Renderer.h"

#include "../Material/UniformBlock.h"
#include "../Material/Program.h"
#include "../Material/Shader.h"
#include "../Lighting/Light.h"

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
	// Multiple-Render-Targes & Framebuffer for Deferred Rendering
	GLuint m_frameBuffer;
	GLuint m_mrt[ MRT_COUNT ];
	////////////////////////////////////////

	// Program and shaders for geometry-stage
	Program* m_progGeomStage;
	Shader* m_vertShaderGeomStage;
	Shader* m_fragShaderGeomStage;
	////////////////////////////////////////

	// Program and shaders for lighting-stage
	Program* m_progLightingStage;
	Shader* m_vertShaderLightingStage;
	Shader* m_fragShaderLightingStage;
	////////////////////////////////////////

	// Program and shaders for shadow-mapping
	Program* m_progShadowMapping;
	Shader* m_vertShaderhadowMapping;
	Shader* m_fragShaderhadowMapping;
	////////////////////////////////////////

	// Uniform-Blocks
	UniformBlock* m_transformBlock;
	////////////////////////////////////////

	// lighting
	Light* m_light;
	////////////////////////////////////////

	bool initFBO();
	bool initGeomStage();
	bool initShadowMapping();
	bool initLightingStage();

	bool renderGeom( Matrix& transf, GeomType* );

};

#endif /* DRRENDERER_H_ */
