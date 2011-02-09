#ifndef RENDERER_H_
#define RENDERER_H_

#include "../Scene/Viewer.h"
#include "../Scene/Instance.h"

#include <list>

#include <assert.h>

#define CHECK_FRAMEBUFFER_STATUS( status ) \
{\
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
	Renderer( Viewer* camera );
	virtual ~Renderer();

	virtual bool initialize() = 0;
	virtual bool shutdown() = 0;
	virtual bool renderFrame( std::list<Instance*>& instances ) = 0;

	void printInfo();

 protected:
	 long long frame;

	 Viewer* m_camera;

	 static bool geomInstanceDistCmp( Instance* a, Instance* b )
	 {
	 	return a->distance < b->distance;
	 }
};

#endif /*STANDARDTRAVERSAL_H_*/
