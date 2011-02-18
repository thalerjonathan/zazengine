/*
 * Light.h
 *
 *  Created on: Feb 4, 2011
 *      Author: jonathan
 */

#ifndef LIGHT_H_
#define LIGHT_H_

#include "../Scene/Viewer.h"

#include <GL/glew.h>

class Light : public Viewer
{
 public:
	static Light* createSpoptLight( float, int, int );
	static Light* createDirectionalLight( int, int );
	static Light* createPointLight( int );

	virtual ~Light();


	GLuint getShadowMap() { return this->m_shadowMap; };
	GLuint* getShadowCubeMap() { return this->m_cubeShadowMap; };

 private:
	Light( int, int );

	GLuint m_shadowMap;
	GLuint m_cubeShadowMap[6];

	bool createShadowMap( int width, int height );
	bool createShadowCubeMap( int width, int height );

};

#endif /* LIGHT_H_ */
