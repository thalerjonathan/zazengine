#include "Vector.h"

#include <math.h>
#include <stdio.h>

#include <iostream>

using namespace std;

Vector::Vector()
{
	this->data[0] = 0;
	this->data[1] = 0;
	this->data[2] = 0;
	this->data[3] = 1;
}

Vector::Vector(const Vector& v)
{
	this->data[0] = v.data[0];
	this->data[1] = v.data[1];
	this->data[2] = v.data[2];
	this->data[3] = v.data[3];
}

Vector::Vector(float x, float y, float z)
{
	this->data[0] = x;
	this->data[1] = y;
	this->data[2] = z;
	this->data[3] = 1;
}

Vector::~Vector()
{
}

float Vector::dotProduct(const Vector& v)
{
	return (this->data[0] * v.data[0] + this->data[1] * v.data[1] + this->data[2] * v.data[2]);
}

Vector Vector::operator-()
{
	Vector vec;
	
	vec.data[0] = -this->data[0];
	vec.data[1] = -this->data[1];
	vec.data[2] = -this->data[2];
	
	return vec;
}

Vector& Vector::operator=(const Vector& v)
{
	Vector newV;
	
	this->data[0] = v.data[0];
	this->data[1] = v.data[1];
	this->data[2] = v.data[2];
	this->data[3] = v.data[3];
	
	return *this;
}

float Vector::length() {

	return sqrt(this->data[0]*this->data[0] + this->data[1]*this->data[1] + this->data[2]*this->data[2]);
}

void Vector::normalize()
{
	float len = this->length();
	if (len) {
		this->data[0] /= len;
		this->data[1] /= len;
		this->data[2] /= len;
	}
}

void Vector::print() const
{
	printf("%+.2f %+.2f %+.2f %+.2f\n", this->data[0], this->data[1], this->data[2], this->data[3]);
}

Vector operator+(const Vector& v1, const Vector& v2)
{
	Vector v;
	
	v.data[0] = v1.data[0] + v2.data[0];
	v.data[1] = v1.data[1] + v2.data[1];
	v.data[2] = v1.data[2] + v2.data[2];
	
	return v;
}

Vector operator-(const Vector& v1, const Vector& v2)
{
	Vector v;
	
	v.data[0] = v1.data[0] - v2.data[0];
	v.data[1] = v1.data[1] - v2.data[1];
	v.data[2] = v1.data[2] - v2.data[2];
	
	return v;
}

Vector operator*(const Vector& v, float s)
{
	Vector mV;
			
	mV.data[0] = v.data[0] * s;
	mV.data[1] = v.data[1] * s;
	mV.data[2] = v.data[2] * s;
		
	return mV;
}

Vector operator*(const Vector& v1, const Vector& v2)
{
	Vector crossProd;

	crossProd.data[0] = v2.data[1] * v1.data[2] - v2.data[2] * v1.data[1];
	crossProd.data[1] = v2.data[2] * v1.data[0] - v2.data[0] * v1.data[2];
	crossProd.data[2] = v2.data[0] * v1.data[1] - v2.data[1] * v1.data[0];

	return crossProd;
}
