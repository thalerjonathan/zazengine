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

#define SHADOW_MAP_WIDTH 	800
#define SHADOW_MAP_HEIGHT	600

#include "Renderer.h"

#include "../Material/UniformBlock.h"
#include "../Material/Program.h"
#include "../Material/Shader.h"
#include "../Lighting/Light.h"

class DRRenderer : public Renderer
{
 public:
	DRRenderer( Viewer* camera );
	virtual ~DRRenderer();

	virtual bool initialize();
	virtual bool shutdown();

	// renders this list of geominstances which must be in front-to-back order
	bool renderFrame( std::list<Instance*>& instances );

 private:
	// Multiple-Render-Targes & Framebuffer for Deferred Rendering
	GLuint m_drFB;
	GLuint m_mrt[ MRT_COUNT ];
	////////////////////////////////////////

	// Program and shaders for geometry-stage
	Program* m_progGeomStage;
	Shader* m_vertGeomStage;
	Shader* m_fragGeomStage;
	////////////////////////////////////////

	// Program and shaders for lighting-stage
	Program* m_progLightingStage;
	Shader* m_vertLightingStage;
	Shader* m_fragLightingStage;
	////////////////////////////////////////

	// Program and shaders for shadow-mapping
	Program* m_progShadowMapping;
	Shader* m_vertShadowMapping;
	Shader* m_fragShadowMapping;

	GLuint m_shadowMap;
	GLuint m_shadowMappingFB;
	////////////////////////////////////////

	// Uniform-Blocks
	UniformBlock* m_transformBlock;
	////////////////////////////////////////

	// lighting and shadowing
	glm::mat4 m_unitCubeMatrix;
	glm::mat4 m_lightSpace;

	Light* m_light;
	////////////////////////////////////////

	bool initFBO();
	bool initGeomStage();
	bool initLightingStage();
	bool initShadowMapping();
	bool initUniformBlocks();

	bool renderShadowMap( std::list<Instance*>& instances );
	bool renderGeometryStage( std::list<Instance*>& instances );
	bool renderLightingStage( std::list<Instance*>& instances );

	bool renderInstances( Viewer* viewer, std::list<Instance*>& instances );
	bool renderGeom( Viewer* viewer, Instance* parent, GeomType* geom );

	bool showTexture( GLuint texID, int quarter );

};

#endif /* DRRENDERER_H_ */
