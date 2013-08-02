/*
 * RenderingContext.h
 *
 *  Created on: 13.03.2013
 *      Author: Jonathan Thaler
 */
#ifndef RENDERING_CONTEXT_H_
#define RENDERING_CONTEXT_H_

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <string>

class RenderingContext
{
	public:
		static bool initialize( const std::string&, int, int, bool );
		static bool shutdown();
		static RenderingContext& getRef() { return *RenderingContext::instance; };

		bool toggleFullscreen();

		void* getHandle() { return this->m_hWnd; };

		int getWidth() { return this->m_windowWidth; };
		int getHeight() { return this->m_windowHeight; };

		bool isActive() { return this->m_activeFlag; };
		
		bool swapBuffers();

	private:
		static RenderingContext* instance;

		RenderingContext( const std::string& title, int width, int height, bool fullScreenFlag );
		~RenderingContext();

		std::string m_windowTitle;

		bool m_fullScreen;
		
		int m_windowWidth;
		int m_windowHeight;

		bool m_activeFlag;

		void* m_hWnd;
		GLFWwindow* m_window;

		void resize( int, int );

		static bool initGLFW();
		static bool initGlew();

};

#endif /* RENDERING_CONTEXT_H_ */