#ifndef RENDERER_H_
#define RENDERER_H_

#include "../Scene/Camera.h"
#include "../Geometry/GeomInstance.h"
#include "../Geometry/GeomSkyBox.h"

#include <assert.h>

#define CHECK_FRAMEBUFFER_STATUS() \
{\
 GLenum status; \
 status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
 switch(status) { \
 case GL_FRAMEBUFFER_COMPLETE_EXT: \
   break; \
 case GL_FRAMEBUFFER_UNSUPPORTED_EXT: \
   fprintf(stderr,"framebuffer GL_FRAMEBUFFER_UNSUPPORTED_EXT\n");\
    /* you gotta choose different formats */ \
   assert(0); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: \
   fprintf(stderr,"framebuffer INCOMPLETE_ATTACHMENT\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: \
   fprintf(stderr,"framebuffer FRAMEBUFFER_MISSING_ATTACHMENT\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: \
   fprintf(stderr,"framebuffer FRAMEBUFFER_DIMENSIONS\n");\
   break; \
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: \
   fprintf(stderr,"framebuffer INCOMPLETE_FORMATS\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: \
   fprintf(stderr,"framebuffer INCOMPLETE_DRAW_BUFFER\n");\
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: \
   fprintf(stderr,"framebuffer INCOMPLETE_READ_BUFFER\n");\
   break; \
 case GL_FRAMEBUFFER_BINDING_EXT: \
   fprintf(stderr,"framebuffer BINDING_EXT\n");\
   break; \
/*
 case GL_FRAMEBUFFER_STATUS_ERROR_EXT: \
   fprintf(stderr,"framebuffer STATUS_ERROR\n");\
   break; \
*/ \
 default: \
   /* programming error; will fail on all hardware */ \
   assert(0); \
 }\
}


class Renderer
{
 public:
	Renderer( Camera& camera, std::string& );
	virtual ~Renderer();

	virtual bool initialize() = 0;
	virtual void renderFrame( GeomInstance* ) = 0;

	void printInfo();

 protected:
	 long long renderedFaces;
	 int renderedInstances;
	 int occludedInstances;
	 int culledInstances;

	 long long frame;

	 Camera& camera;
	 GeomSkyBox* skyBox;

	 static bool geomInstanceDistCmp( GeomInstance* a, GeomInstance* b )
	 {
	 	return a->distance < b->distance;
	 }

	 void createFBO(GLuint*, GLuint*);
};

#endif /*STANDARDTRAVERSAL_H_*/
