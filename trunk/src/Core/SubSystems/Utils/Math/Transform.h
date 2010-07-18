#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include "Vector.h"
#include "Matrix.h"

class Transform
{
 public:
	Transform();
	~Transform();
	 
	Matrix matrix;
	 
	void setPosition(const Vector&);
    void setPosition(float, float, float);
		
	void changeHeading(float); /**< \brief Changes heading of object (i.e. turn left-right) */
	void changePitch(float); /**< \brief Changes Pitch of object (i.e.up-down) */
	void changeRoll(float); /**< \brief Changes roll of object (i.e. roll left-right) */
	void strafeForward(float); /**< \brief Moves the object forward */
	void strafeRight(float); /**< \brief Moves the object to its right */
	void strafeUp(float); /**< \brief Moves the object upwards */
	void strafe(float*, float); /**< \brief Moves the object into given direction */
};

#endif /*TRANSFORMNODE_H_*/
