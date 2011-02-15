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
 * Iterate over each light and apply it through the rendering of a quad with the size of the screen.
 * Additively blend the results of each light together.
 *
 * Shadowing
 * This renderer utilizes ShadowMaps as shadow-algorithm. The scene is rendererd from the viewpoint of
 * the light into a depth-texture ( managed by the light ). Shadowing itself happens in the lighting-stage.
 * The screen-space coordinate is transformed back into world-space (using the depth) and then transformed
 * into light-space. Furthermore the light-space coordinate is transformed into 0-1 to be able to access
 * the shadowmap like a texture ( using textureProj ).
 *
 * Lighting
 * because this is a deferred renderer, lighting occurs in screen-space. for this the normals must be
 * in screen-space and the lights position and direction too.
 * Very imporant is that light-direction and position must be transformed with the camera too because
 * each light is also part of the scene and must be placed in world-coordinates with its modeling-transformations.
 * During the lighting-pass each light is transformed into its world-coordinates by multiplying its
 * modeling-matrix with the viewing matrix of the camera. If one doesn't do this, the light sticks with the
 * camera (maybe this is desired...)
 */

/* Errors:
 * - Visibility in G-Buffer not correct because we don't render against a z-buffer. so the object first in the
 * 	 renderlist shows up over the others.
 * 		-> add a depth-buffer to the framebuffer? ( http://www.gamedev.net/topic/578084-depth-buffer-and-deferred-rendering/ )
 *
 * - Shadowmap display: glTexParameteri(texture_target, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
 * - Shadowing artefacts:
 *		-> employ Face-Culling: cull front-faces during shadowmap generation
 *
 * - Lighting and Normals:
 * 		-> check space of normals and camera: normals must be provided in screen-space and camera position
 * 			and direction must be transformed to screen-space too. LIGHTING MUST ALWAYS HAPPEN IN THE SAME
 * 		    SPACE.
 * 		-> something wrong with normals transformation ( maybe they're not correctly loaded in geometryfactory )
 *		-> lights must be transformed by camera-viewingMatrix to world-coordinates otherwise they stick with camera
 *
 * - Correct Deferred Shadowing:
 * 		-> something still wrong with the shadowing transformation in lighting Fragment-shader
 *
 * Questions:
 * - am i in the right space?
 * - do i mix up spaces (e.g. screen-space and view-space or clipping space (after applying mvp matrix ) with projection space ( after perspective div to NDC ) )
 * - is the data correct i receive?
 * - where does the data i receive come from?
 * - what is the data i receive?
 * - where does the data go to i transform?
 * - what happens with the data i transformed, send away and received somewhere else in between?
 */

/* TODO:
 * - Enhance Deferred Shadowing (reduce artifacts and implement soft-shadows )
 * - Introduce multiple lights: each light contributes ADDITIVELY to the framebuffe, solve this
 * - Material-Model: diffuse texturing, transparency, metal, SubSurfaceScattering!!!
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
