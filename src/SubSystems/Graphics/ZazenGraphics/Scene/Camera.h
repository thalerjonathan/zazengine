#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>

#include "../../../../Core/Utils/Math/Orientation.h"

enum CullResult {
	INSIDE = 0,
	OUTSIDE,
	INTERSECTING
};

class Camera : public Orientation
{
 public:
	Camera( float, int, int );
	~Camera();

	glm::mat4 m_viewingMatrix;
	glm::mat4 m_projectionMatrix;

    void setupPerspective();
    void setupOrtho();

    int getHeight() { return this->height; };
    int getWidth() { return this->width; };

	void changeNearClip(float);
	void changeFarClip(float);
	
	void resize(int, int);
	void changeFov(float);
	
	/*
	CullResult cullBB( const glm::vec3&, const glm::vec3& );
	CullResult cullSphere( const glm::vec3&, float );
	*/
	
 private:
	float width;
	float height;

	float angle;
	float ratio;

	float nearDist;
	float farDist;
	
	float frustum[6][4];
	
	void recalculateFrustum();
	
};

#endif /*CAMERA_H_*/
