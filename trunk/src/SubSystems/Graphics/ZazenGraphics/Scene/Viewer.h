/*
 * Viewer.h
 *
 *  Created on: Feb 9, 2011
 *      Author: jonathan
 */


#ifndef VIEWER_H_
#define VIEWER_H_

#include "../../../../Core/Utils/Math/Orientation.h"

/* Can be used as the view-point of a camera or light
 *
 * in opengl the camera is alwayx in the 0,0,0 origin and points down
 * the -Z achsis. the camera itself is not modified but instead one has
 * to apply the inverse operation on the viewing matrix to achive a
 * camera/viewer transformation
 */

class Viewer : public Orientation
{
 public:
	enum CullResult {
		INSIDE = 0,
		OUTSIDE,
		INTERSECTING
	};

	Viewer( float, int, int );
	~Viewer();

	glm::mat4 m_PVMatrix;
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

	CullResult cullBB( const glm::vec3&, const glm::vec3& );

 protected:
	// overridden from Orientation
	virtual void matrixChanged();
	
 private:
	float width;
	float height;

	float angle;
	float ratio;

	float nearDist;
	float farDist;
	
};

#endif /*VIEWER_H_*/
