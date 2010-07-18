#include "Matrix.h"

#include <string.h>
#include <math.h>
#include <stdio.h>

#include <iostream>

using namespace std;

Matrix::Matrix()
{
	this->identiy();
}

Matrix::~Matrix()
{
}

Matrix::Matrix(float* data)
{
	memcpy(this->data, data, sizeof(this->data));
}

Matrix::Matrix(const Matrix& cpy)
{
	memcpy(this->data, cpy.data, sizeof(this->data));
}

void Matrix::identiy()
{
	memset(this->data, 0, sizeof(this->data));
		
	this->data[0] = 1;
	this->data[5] = 1;
	this->data[10] = 1;
	this->data[15] = 1;
}

void Matrix::transpose3x3()
{
	float tmp = this->data[1];
	this->data[1] = this->data[4];
	this->data[4] = tmp;
	
	tmp = this->data[2];
	this->data[2] = this->data[8];
	this->data[8] = tmp;
	
	tmp = this->data[6];
	this->data[6] = this->data[9];
	this->data[9] = tmp;
}

void  Matrix::set(float* m)
{
	memcpy(this->data, m, sizeof(this->data));
}

void Matrix::multiply(Matrix& mat)
{
	this->multiply(mat.data);
}

void Matrix::multiply(float* mat)
{
	float result[16];
		
	result[0] = this->data[0] * mat[0] + this->data[1] * mat[4] + this->data[2] * mat[8] + this->data[3] * mat[12];
	result[1] = this->data[0] * mat[1] + this->data[1] * mat[5] + this->data[2] * mat[9] + this->data[3] * mat[13];
	result[2] = this->data[0] * mat[2] + this->data[1] * mat[6] + this->data[2] * mat[10] + this->data[3] * mat[14];
	result[3] = this->data[0] * mat[3] + this->data[1] * mat[7] + this->data[2] * mat[11] + this->data[3] * mat[15];
	
	result[4] = this->data[4] * mat[0] + this->data[5] * mat[4] + this->data[6] * mat[8] + this->data[7] * mat[12];
	result[5] = this->data[4] * mat[1] + this->data[5] * mat[5] + this->data[6] * mat[9] + this->data[7] * mat[13];
	result[6] = this->data[4] * mat[2] + this->data[5] * mat[6] + this->data[6] * mat[10] + this->data[7] * mat[14];
	result[7] = this->data[4] * mat[3] + this->data[5] * mat[7] + this->data[6] * mat[11] + this->data[7] * mat[15];

	result[8] = this->data[8] * mat[0] + this->data[9] * mat[4] + this->data[10] * mat[8] + this->data[11] * mat[12];
	result[9] = this->data[8] * mat[1] + this->data[9] * mat[5] + this->data[10] * mat[9] + this->data[11] * mat[13];
	result[10] = this->data[8] * mat[2] + this->data[9] * mat[6] + this->data[10] * mat[10] + this->data[11] * mat[14];
	result[11] = this->data[8] * mat[3] + this->data[9] * mat[7] + this->data[10] * mat[11] + this->data[11] * mat[15];

	result[12] = this->data[12] * mat[0] + this->data[13] * mat[4] + this->data[14] * mat[8] + this->data[15] * mat[12];
	result[13] = this->data[12] * mat[1] + this->data[13] * mat[5] + this->data[14] * mat[9] + this->data[15] * mat[13];
	result[14] = this->data[12] * mat[2] + this->data[13] * mat[6] + this->data[14] * mat[10] + this->data[15] * mat[14];
	result[15] = this->data[12] * mat[3] + this->data[13] * mat[7] + this->data[14] * mat[11] + this->data[15] * mat[15];

	memcpy(this->data, result, sizeof(this->data));
}

void Matrix::multiplyInv(Matrix& mat)
{
	this->multiplyInv(mat.data);
}

void Matrix::multiplyInv(float* matrix)
{
	float result[16];
	
	result[0] = matrix[0] * this->data[0] + matrix[1] * this->data[4] + matrix[2] * this->data[8] + matrix[3] * this->data[12];
	result[1] = matrix[0] * this->data[1] + matrix[1] * this->data[5] + matrix[2] * this->data[9] + matrix[3] * this->data[13];
	result[2] = matrix[0] * this->data[2] + matrix[1] * this->data[6] + matrix[2] * this->data[10] + matrix[3] * this->data[14];
	result[3] = matrix[0] * this->data[3] + matrix[1] * this->data[7] + matrix[2] * this->data[11] + matrix[3] * this->data[15];
	
	result[4] = matrix[4] * this->data[0] + matrix[5] * this->data[4] + matrix[6] * this->data[8] + matrix[7] * this->data[12];
	result[5] = matrix[4] * this->data[1] + matrix[5] * this->data[5] + matrix[6] * this->data[9] + matrix[7] * this->data[13];
	result[6] = matrix[4] * this->data[2] + matrix[5] * this->data[6] + matrix[6] * this->data[10] + matrix[7] * this->data[14];
	result[7] = matrix[4] * this->data[3] + matrix[5] * this->data[7] + matrix[6] * this->data[11] + matrix[7] * this->data[15];

	result[8] = matrix[8] * this->data[0] + matrix[9] * this->data[4] + matrix[10] * this->data[8] + matrix[11] * this->data[12];
	result[9] = matrix[8] * this->data[1] + matrix[9] * this->data[5] + matrix[10] * this->data[9] + matrix[11] * this->data[13];
	result[10] = matrix[8] * this->data[2] + matrix[9] * this->data[6] + matrix[10] * this->data[10] + matrix[11] * this->data[14];
	result[11] = matrix[8] * this->data[3] + matrix[9] * this->data[7] + matrix[10] * this->data[11] + matrix[11] * this->data[15];

	result[12] = matrix[12] * this->data[0] + matrix[13] * this->data[4] + matrix[14] * this->data[8] + matrix[15] * this->data[12];
	result[13] = matrix[12] * this->data[1] + matrix[13] * this->data[5] + matrix[14] * this->data[9] + matrix[15] * this->data[13];
	result[14] = matrix[12] * this->data[2] + matrix[13] * this->data[6] + matrix[14] * this->data[10] + matrix[15] * this->data[14];
	result[15] = matrix[12] * this->data[3] + matrix[13] * this->data[7] + matrix[14] * this->data[11] + matrix[15] * this->data[15];

	memcpy(this->data, result, sizeof(this->data));
}

void Matrix::transform(Vector& point) const
{
	float result[4];
	
	result[0] = this->data[0] * point[0] + this->data[4] * point[1] + this->data[8] * point[2] + this->data[12] * point[3];
	result[1] = this->data[1] * point[0] + this->data[5] * point[1] + this->data[9] * point[2] + this->data[13] * point[3];;
	result[2] = this->data[2] * point[0] + this->data[6] * point[1] + this->data[10] * point[2] + this->data[14] * point[3];
	result[3] = this->data[3] * point[0] + this->data[7] * point[1] + this->data[11] * point[2] + this->data[15] * point[3];
	
	point.data[0] = result[0];
	point.data[1] = result[1];
	point.data[2] = result[2];
	point.data[3] = result[3];
}

Matrix& Matrix::operator=(const Matrix& cpy)
{
	memcpy(this->data, cpy.data, sizeof(this->data));
	
	return *this;
}

void Matrix::print()
{
	printf("%+.2f %+.2f %+.2f %+.2f\n", this->data[0], this->data[4], this->data[8], this->data[12]);
	printf("%+.2f %+.2f %+.2f %+.2f\n", this->data[1], this->data[5], this->data[9], this->data[13]);
	printf("%+.2f %+.2f %+.2f %+.2f\n", this->data[2], this->data[6], this->data[10], this->data[14]);
	printf("%+.2f %+.2f %+.2f %+.2f\n", this->data[3], this->data[7], this->data[11], this->data[15]);
	
	cout << endl;
}

void Matrix::rotate(float u, float v, float w, float angle)
{
	float a = this->data[12];
	float b = this->data[13];
	float c = this->data[14];
	
	float L = sqrt(u*u + v*v + w*w);
	float invL2 = 1/(L*L); // optimization: all therms need to be divided by L² => we multiply by 1/L²
	float cosA = cos(angle);
	float sinA = sin(angle);
	float u2 = u*u;
	float v2 = v*v;
	float w2 = w*w;
	float invCosA = 1 - cosA;

	float matrix[16];
	
	matrix[0] = (u2 + (v2 + w2) * cosA) * invL2;
	matrix[1] = (u*v * invCosA + w*L*sinA) * invL2;
	matrix[2] = (u*w * invCosA - v*L*sinA) * invL2;
	matrix[3] = 0;
	
	matrix[4] = (u*v * invCosA - w*L*sinA) * invL2;
	matrix[5] = (v2 + (u2 + w2) * cosA) * invL2;
	matrix[6] = (v*w * invCosA + u*L*sinA) * invL2;
	matrix[7] = 0;
	
	matrix[8] = (u*w * invCosA + v*L*sinA) * invL2;
	matrix[9] = (v*w * invCosA - u*L*sinA) * invL2;
	matrix[10] = (w2 + (u2 + v2) * cosA) * invL2;
	matrix[11] = 0;
	
	matrix[12] = (a*(v2 + w2) - u*(b*v + c*w) + (u*(b*v + c*w) - a*(v2 + w2)) * cosA + (b*w - c*v) * L * sinA) * invL2;
	matrix[13] = (b*(u2 + w2) - v*(a*u + c*w) + (v*(a*u + c*w) - b*(u2 + w2)) * cosA + (c*u - a*w) * L * sinA) * invL2;
	matrix[14] = (c*(u2 + v2) - w*(a*u + b*v) + (w*(a*u + b*v) - c*(u2 + v2)) * cosA + (a*v - b*u) * L * sinA) * invL2;
	matrix[15] = 1;
	
	this->multiply(matrix);
}

void Matrix::rotateInverse(float u, float v, float w, float angle)
{	
	float L = sqrt(u*u + v*v + w*w);
	float invL2 = 1/(L*L); // optimization: all therms need to be divided by L² => we multiply by 1/L²
	float cosA = cos(angle);
	float sinA = sin(angle);
	float u2 = u*u;
	float v2 = v*v;
	float w2 = w*w;
	float invCosA = 1 - cosA;

	float matrix[16];
	
	matrix[0] = (u2 + (v2 + w2) * cosA) * invL2;
	matrix[4] = (u*v * invCosA + w*L*sinA) * invL2;	// transposed: 1-4
	matrix[8] = (u*w * invCosA - v*L*sinA) * invL2;	// transposed: 2-8
	matrix[3] = 0;
	
	matrix[1] = (u*v * invCosA - w*L*sinA) * invL2;
	matrix[5] = (v2 + (u2 + w2) * cosA) * invL2;
	matrix[9] = (v*w * invCosA + u*L*sinA) * invL2;	// transposed: 6-9
	matrix[7] = 0;
	
	matrix[2] = (u*w * invCosA + v*L*sinA) * invL2;
	matrix[6] = (v*w * invCosA - u*L*sinA) * invL2;
	matrix[10] = (w2 + (u2 + v2) * cosA) * invL2;
	matrix[11] = 0;
	
	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;
	
	this->multiply(matrix);
}
