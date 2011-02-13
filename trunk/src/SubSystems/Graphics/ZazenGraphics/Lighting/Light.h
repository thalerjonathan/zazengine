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
	static Light* createLight( float, int, int );
	virtual ~Light();

	GLuint getShadowMapID() { return this->m_shadowMapID; };

 private:
	Light( float, int, int );

	GLuint m_shadowMapID;

};

#endif /* LIGHT_H_ */
