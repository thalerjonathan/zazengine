#include <GL/glew.h>

#include "Transform.h"

#include <iostream>
#include <string.h>

using namespace std;

Transform::Transform()
{
	this->matrix.identiy();
}

Transform::~Transform()
{
}

void Transform::setPosition(const Vector& v)
{
	memcpy(&this->matrix.data[12], v.data, 3 * sizeof(float));
}

void Transform::setPosition(float x, float y, float z)
{
	this->matrix.data[12] = x;
	this->matrix.data[13] = y;
	this->matrix.data[14] = z;
}

void Transform::changePitch(float angle)
{
	this->matrix.rotate(this->matrix[0], this->matrix[1], this->matrix[2], angle);
}

void Transform::changeHeading(float angle)
{
	this->matrix.rotate(this->matrix[4], this->matrix[5], this->matrix[6], angle);
}

void Transform::changeRoll(float angle)
{
	this->matrix.rotate(this->matrix[8], this->matrix[9], this->matrix[10], angle);
}

void Transform::strafeForward(float units)
{
	this->matrix.data[12] += units * this->matrix[8];
	this->matrix.data[13] += units * this->matrix[9];
	this->matrix.data[14] += units * this->matrix[10];
}

void Transform::strafeRight(float units)
{
	this->matrix.data[12] += units * this->matrix[0];
	this->matrix.data[13] += units * this->matrix[1];
	this->matrix.data[14] += units * this->matrix[2];
}

void Transform::strafeUp(float units)
{
	this->matrix.data[12] += units * this->matrix[4];
	this->matrix.data[13] += units * this->matrix[5];
	this->matrix.data[14] += units * this->matrix[6];
}
