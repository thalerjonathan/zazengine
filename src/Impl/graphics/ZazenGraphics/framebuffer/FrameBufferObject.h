#ifndef _FRAMEBUFFER_OBJECT_H_
#define _FRAMEBUFFER_OBJECT_H_

#include "RenderTarget.h"

#include <GL/glew.h>

#include <vector>

#ifdef _DEBUG
	#define CHECK_FRAMEBUFFER_DEBUG FrameBufferObject::checkStatus();
#else
	#define CHECK_FRAMEBUFFER_DEBUG
#endif

class FrameBufferObject
{
	public:
		static FrameBufferObject* create();
		static bool destroy( FrameBufferObject* );

		static bool checkStatus();

		bool attachTarget( RenderTarget* );
		bool attachTargetTemp( RenderTarget* );
		bool attachTargetTempCubeFace( RenderTarget*, unsigned int );

		bool drawAllBuffers();
		bool drawBuffer( unsigned int );
		bool drawBuffers( std::vector<unsigned int> );
		bool drawNone();

		bool copyDepthToTarget( RenderTarget* );

		bool bind();
		bool unbind();

		bool bindAllTargets();
		bool bindTargets( std::vector<unsigned int> );
		bool bindTarget( unsigned int );

		bool clearAll();

		const std::vector<RenderTarget*>& getAttachedTargets() { return this->m_attachedTargets; };
		RenderTarget* getAttachedDepthTarget() { return this->m_depthTarget; };

	private:
		FrameBufferObject( GLuint );
		~FrameBufferObject();

		GLuint m_id;

		GLuint m_depthBuffer;	

		RenderTarget* m_depthTarget;

		std::vector<GLenum> m_colorBufferTargets;
		std::vector<GLuint> m_colorBuffers;
		
		std::vector<RenderTarget*> m_attachedTargets;
};

#endif /* _FRAMEBUFFER_OBJECT_H_ */