/*
 * DRRenderer.h
 *
 *  Created on: Jan 16, 2011
 *      Author: jonathan
 */

#ifndef DRRENDERER_H_
#define DRRENDERER_H_

// number of rendering targets for deferred renderer
#define MRT_COUNT 3

#define SHADOW_MAP_WIDTH 	800
#define SHADOW_MAP_HEIGHT	600

#include "Renderer.h"

#include "../Material/UniformBlock.h"
#include "../Material/Program.h"
#include "../Material/Shader.h"
#include "../Lighting/Light.h"

/* Deferred Rendering
 * Geometry-Stage:
 * Renders diffuse color ( texturing will be applied here ), depth and normals ( could also come from a normalmap )
 * into 3 rendering target. Those targets will be used by the LightingStage to produce the final result.
 *
 * Lighting-Stage:
 * Uses the rendering targets to calculate the final color of each fragment. See Shadowing how shadowing is applied
 *
 * Shadowing
 * This renderer utilizes ShadowMaps as shadow-algorithm. The scene is rendererd from the viewpoint of
 * the light into a depth-texture ( managed by the light ). Shadowing itself happens in the lighting-stage.
 * The screen-space coordinate is transformed back into world-space (using the depth) and then transformed
 * into light-space. Furthermore the light-space coordinate is transformed into 0-1 to be able to access
 * the shadowmap like a texture ( using textureProj ).
 *
 * Lighting
 * No Lighting implemented yet
 */

/* Errors:
 * - Correct Deferred Shadowing: something still wrong with the shadowing transformation in lighting Fragment-shader
 * - Normals: something wrong with normals transformation ( maybe they're not correctly loaded in geometryfactory )
 */

/* TODO:
 * - Enhance Deferred Shadowing (reduce artifacts and implement soft-shadows )
 * - Lighting: implement a lighting model ( e.g. phong )
 * - Introduce multiple lights: each light contributes ADDITIVELY to the framebuffe, solve this
 * - Material-Model: diffuse texturing, transparency, metal
 * - Reflections: would be nice to have reflections in this renderer too
 */
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
	GLenum m_buffers[ MRT_COUNT ];
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

	GLuint m_shadowMappingFB;
	////////////////////////////////////////

	// Uniform-Blocks
	UniformBlock* m_mvpTransformBlock;
	UniformBlock* m_lightDataBlock;
	////////////////////////////////////////

	// lighting and shadowing
	std::vector<Light*> m_lights;
	////////////////////////////////////////

	// utils matrix
	glm::mat4 m_unitCubeMatrix;

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
