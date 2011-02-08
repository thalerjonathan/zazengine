#include "Camera.h"

#include <GL/glew.h>

#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>
#include <iostream>

#define ANG2RAD 3.14159265358979323846/180.0

using namespace std;

Camera::Camera( float angle, int width, int height )
	: Orientation( m_viewingMatrix )
{
	this->width = width;
	this->height = height;

	this->angle = angle;
	this->ratio = (float) this->width / (float) this->height;
	
	this->nearDist = 1;
	this->farDist = 1000000;

	this->setupPerspective();
}

Camera::~Camera()
{
}

void
Camera::setupPerspective()
{
	this->m_projectionMatrix = glm::perspective( this->angle, this->ratio, this->nearDist, this->farDist );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glLoadMatrixf( &this->m_projectionMatrix[ 0 ][ 0 ] );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Camera::setupOrtho()
{
	this->m_projectionMatrix = glm::ortho( 0.0f, this->width, this->height, 0.0f, -1.0f, 1.0f );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glLoadMatrixf( &this->m_projectionMatrix[ 0 ][ 0 ] );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void
Camera::changeNearClip(float dist)
{
	this->nearDist = dist;
	
	this->setupPerspective();
}

void
Camera::changeFarClip(float dist)
{
	this->farDist = dist;
	
	this->setupPerspective();
}

void
Camera::resize(int width, int height)
{
	this->ratio = (float) width / (float) height;
	
	this->setupPerspective();
}

void
Camera::changeFov(float angle)
{
	this->angle = angle;
	
	this->setupPerspective();
}

void
Camera::recalculateFrustum()
{
	/*
	float t;
	Matrix clip(this->viewingMatrix);
	clip.multiply(this->projection);
		
	// RIGHT plane
	this->frustum[0][0] = clip[3] - clip[0];
	this->frustum[0][1] = clip[7] - clip[4];
	this->frustum[0][2] = clip[11] - clip[8];
	this->frustum[0][3] = clip[15] - clip[12];

	t = 1 / sqrt(this->frustum[0][0] * this->frustum[0][0] + this->frustum[0][1] * this->frustum[0][1] + this->frustum[0][2] * this->frustum[0][2]);
	this->frustum[0][0] *= t;
	this->frustum[0][1] *= t;
	this->frustum[0][2] *= t;
	this->frustum[0][3] *= t;

	// LEFT plane
	this->frustum[1][0] = clip[3] + clip[0];
	this->frustum[1][1] = clip[7] + clip[4];
	this->frustum[1][2] = clip[11] + clip[8];
	this->frustum[1][3] = clip[15] + clip[12];

	t = 1 / sqrt(this->frustum[1][0] * this->frustum[1][0] + this->frustum[1][1] * this->frustum[1][1] + this->frustum[1][2] * this->frustum[1][2]);
	this->frustum[1][0] *= t;
	this->frustum[1][1] *= t;
	this->frustum[1][2] *= t;
	this->frustum[1][3] *= t;

	// BOTTOM plane
	this->frustum[2][0] = clip[3] + clip[1];
	this->frustum[2][1] = clip[7] + clip[5];
	this->frustum[2][2] = clip[11] + clip[9];
	this->frustum[2][3] = clip[15] + clip[13];

	t = 1 / sqrt(this->frustum[2][0] * this->frustum[2][0] + this->frustum[2][1] * this->frustum[2][1] + this->frustum[2][2] * this->frustum[2][2]);
	this->frustum[2][0] *= t;
	this->frustum[2][1] *= t;
	this->frustum[2][2] *= t;
	this->frustum[2][3] *= t;

	// TOP plane
	this->frustum[3][0] = clip[3] - clip[1];
	this->frustum[3][1] = clip[7] - clip[5];
	this->frustum[3][2] = clip[11] - clip[9];
	this->frustum[3][3] = clip[15] - clip[13];

	t = 1 / sqrt(this->frustum[3][0] * this->frustum[3][0] + this->frustum[3][1] * this->frustum[3][1] + this->frustum[3][2] * this->frustum[3][2]);
	this->frustum[3][0] *= t;
	this->frustum[3][1] *= t;
	this->frustum[3][2] *= t;
	this->frustum[3][3] *= t;

	// FAR plane
	this->frustum[4][0] = clip[3] - clip[2];
	this->frustum[4][1] = clip[7] - clip[6];
	this->frustum[4][2] = clip[11] - clip[10];
	this->frustum[4][3] = clip[15] - clip[14];

	t = 1 / sqrt(this->frustum[4][0] * this->frustum[4][0] + this->frustum[4][1] * this->frustum[4][1] + this->frustum[4][2] * this->frustum[4][2]);
	this->frustum[4][0] *= t;
	this->frustum[4][1] *= t;
	this->frustum[4][2] *= t;
	this->frustum[4][3] *= t;

	// NEAR plane
	this->frustum[5][0] = clip[3] + clip[2];
	this->frustum[5][1] = clip[7] + clip[6];
	this->frustum[5][2] = clip[11] + clip[10];
	this->frustum[5][3] = clip[15] + clip[14];

	t = 1 / sqrt(this->frustum[5][0] * this->frustum[5][0] + this->frustum[5][1] * this->frustum[5][1] + this->frustum[5][2] * this->frustum[5][2]);
	this->frustum[5][0] *= t;
	this->frustum[5][1] *= t;
	this->frustum[5][2] *= t;
	this->frustum[5][3] *= t;

	cout << "RIGHT Plane" << endl;
	cout << this->frustum[0][0] << "/" << this->frustum[0][1] << "/" << this->frustum[0][2] << ") d=" << this->frustum[0][3] << endl;

	cout << "LEFT Plane" << endl;
	cout << this->frustum[1][0] << "/" << this->frustum[1][1] << "/" << this->frustum[1][2] << ") d=" << this->frustum[1][3] << endl;

	cout << "BOTTOM Plane" << endl;
	cout << this->frustum[2][0] << "/" << this->frustum[2][1] << "/" << this->frustum[2][2] << ") d=" << this->frustum[2][3] << endl;

	cout << "TOP Plane" << endl;
	cout << this->frustum[3][0] << "/" << this->frustum[3][1] << "/" << this->frustum[3][2] << ") d=" << this->frustum[3][3] << endl;

	cout << "FAR Plane" << endl;
	cout << this->frustum[4][0] << "/" << this->frustum[4][1] << "/" << this->frustum[4][2] << ") d=" << this->frustum[4][3] << endl;

	cout << "NEAR Plane" << endl;
	cout << this->frustum[5][0] << "/" << this->frustum[5][1] << "/" << this->frustum[5][2] << ") d=" << this->frustum[5][3] << endl;
	*/
}

CullResult
Camera::cullBB( const glm::vec3& bbMin, const glm::vec3& bbMax )
{
	return INSIDE;

	int c;
	int c2 = 0;

	for(int p = 0; p < 6; p++) {
		c = 0;

		if(frustum[p][0] * bbMin[0] + frustum[p][1] * bbMin[1] + frustum[p][2] * bbMin[2] + frustum[p][3] > 0 )
			c++;
		if(frustum[p][0] * bbMax[0] + frustum[p][1] * bbMin[0] + frustum[p][2] * bbMin[2] + frustum[p][3] > 0 )
			c++;
		if(frustum[p][0] * bbMin[0] + frustum[p][1] * bbMax[1] + frustum[p][2] * bbMin[2] + frustum[p][3] > 0 )
			c++;
		if(frustum[p][0] * bbMax[0] + frustum[p][1] * bbMax[1] + frustum[p][2] * bbMin[2] + frustum[p][3] > 0 )
			c++;
		if(frustum[p][0] * bbMin[0] + frustum[p][1] * bbMin[0] + frustum[p][2] * bbMax[2] + frustum[p][3] > 0 )
			c++;
		if(frustum[p][0] * bbMax[0] + frustum[p][1] * bbMin[0] + frustum[p][2] * bbMax[2] + frustum[p][3] > 0 )
			c++;
		if(frustum[p][0] * bbMin[0] + frustum[p][1] * bbMax[1] + frustum[p][2] * bbMax[2] + frustum[p][3] > 0 )
			c++;
		if(frustum[p][0] * bbMax[0] + frustum[p][1] * bbMax[1] + frustum[p][2] * bbMax[2] + frustum[p][3] > 0 )
			c++;

		if(c == 0)
			return OUTSIDE;

		if(c == 8)
			c2++;
	}

	return (c2 == 6) ? INSIDE : INTERSECTING;
}

CullResult
Camera::cullSphere( const glm::vec3& pos, float radius )
{
	int c = 0;
	float d;

	for(int p = 0; p < 6; p++ ) {
		d = frustum[p][0] * pos[0] + frustum[p][1] * pos[1] + frustum[p][2] * pos[2] + frustum[p][3];
		if(d <= -radius)
			return OUTSIDE;
		if(d > radius)
	         c++;
	}

	return (c == 6) ? INSIDE : INTERSECTING;
}
