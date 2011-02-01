#ifndef GEOMPLANE_H_
#define GEOMPLANE_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include "GeomType.h"
#include "../Material/Texture.h"
#include <iostream>

class GeomPlane : public GeomType
{
 public:
	GeomPlane(float l, float w)
	{
		this->length = l / 2;
		this->width = w / 2;
	};

	virtual ~GeomPlane() {};

	bool render()
	{
		glBegin(GL_QUADS);
			glNormal3f(0.0, 1.0, 0.0); glTexCoord2f(0.0f, 0.0f); glVertex3f(-this->length, 0, -this->width);	// Top Right Of The Texture and Quad
			glNormal3f(0.0, 1.0, 0.0); glTexCoord2f(1.0f, 0.0f); glVertex3f( this->length, 0, -this->width);	// Top Left Of The Texture and Quad
			glNormal3f(0.0, 1.0, 0.0); glTexCoord2f(1.0f, 1.0f); glVertex3f( this->length, 0, this->width);	// Bottom Left Of The Texture and Quad
			glNormal3f(0.0, 1.0, 0.0); glTexCoord2f(0.0f, 1.0f); glVertex3f(-this->length, 0, this->width);	// Bottom Right Of The Texture and Quad
		glEnd();

		return true;
	};

 private:
	float length;
	float width;
};

#endif /* GEOMPLANE_H_ */
