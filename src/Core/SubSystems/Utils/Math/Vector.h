#ifndef VECTOR_H_
#define VECTOR_H_

class Vector
{
 public:
	 Vector();
	 Vector(const Vector&);
	 Vector(float x, float y, float z);
	~Vector();
	
	float data[4];

	Vector operator-();
	float dotProduct(const Vector&);
	Vector& operator=(const Vector&);

	float length();
	void normalize();

	float operator[](int i) const
	{
		return this->data[i];
	}

	void print() const;

	static float distance(const Vector&, const Vector&);
};

Vector operator+(const Vector&, const Vector&);
Vector operator-(const Vector&, const Vector&);
Vector operator*(const Vector&, const Vector&);
Vector operator*(const Vector&, float);

#endif /*VECTOR_H_*/
