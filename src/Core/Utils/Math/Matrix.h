#ifndef MATRIX_H_
#define MATRIX_H_

#include "Vector.h"

class Matrix
{
public:
	Matrix();
	Matrix(float*);
	Matrix(const Matrix&);
	~Matrix();
	
	float data[16];
	
	void set(float*);
	
	void inverse();
	void identiy();
	void transpose3x3();
	
	void multiply(Matrix&);
	void multiply(float*);
	
	void multiplyInv(Matrix&);
	void multiplyInv(float*);
	
	void transform(Vector&) const;
	
	float operator[](int i)
	{
		return this->data[i];
	}
	
	Matrix& operator=(const Matrix&);
	
	void print();
	
	void rotate(float, float, float, float);
	void rotateInverse(float, float, float, float);
	
};

#endif /*MATRIX_H_*/
