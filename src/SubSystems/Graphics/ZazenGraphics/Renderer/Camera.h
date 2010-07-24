#ifndef CAMERA_H_
#define CAMERA_H_

#include "../../../../Core/Utils/Math/Vector.h"
#include "../../../../Core/Utils/Math/Matrix.h"

enum CullResult {
	INSIDE = 0,
	OUTSIDE,
	INTERSECTING
};

class Camera
{
 public:
	Camera(float, int, int);
	~Camera();

	// the modelviewmatrix for the worldtransform
    Matrix modelView;

    void setupPerspective();
    void setupOrtho();

    Vector getPosition();

    int getHeight() { return this->height; };
    int getWidth() { return this->width; };

	void setPosition(float, float, float);

	void changeHeading(float); /**< \brief Changes heading of object (i.e. turn left-right) */
	void changePitch(float); /**< \brief Changes Pitch of object (i.e.up-down) */
	void changeRoll(float); /**< \brief Changes roll of object (i.e. roll left-right) */
	void strafeForward(float); /**< \brief Moves the object forward */
	void strafeRight(float); /**< \brief Moves the object to its right */
	void strafeUp(float); /**< \brief Moves the object upwards */
	void strafe(float*, float); /**< \brief Moves the object into given direction */

	void changeNearClip(float);
	void changeFarClip(float);
	
	void resize(int, int);
	void changeFov(float);
	
	CullResult cullBB(const Vector&, const Vector&);
	CullResult cullSphere(const Vector&, float);
	
 private:
	int width;
	int height;

	float angle;
	float ratio;

	float nearDist;
	float farDist;
	
	float projection[16];
	float frustum[6][4];
	
	void recalculateFrustum();
	
};

#endif /*CAMERA_H_*/
