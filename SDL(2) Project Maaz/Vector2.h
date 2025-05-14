#pragma once
#include <iostream>
#include <math.h>

class Vector2 
{
public:
	float x;
	float y;
	
	Vector2(): x(0), y(0) {}
	Vector2(float x_, float y_) : x(x_),y(y_){}

	void print() 
	{
		std::cout << '(' << coolRound(x,2) << ',' << coolRound(y,2) << ')' << std::endl;
	}

	Vector2 get() 
	{
		return Vector2(x,y);
	}

	Vector2 operator+(const Vector2& vec2) 
	{
		return Vector2(x + vec2.x, y + vec2.y);
	}

	Vector2 operator+(const float add) 
	{
		return Vector2(x + add, y + add);
	}

	void operator+=(const Vector2& vec2)
	{
		x += vec2.x;
		y += vec2.y;
	}

	Vector2 operator-(const Vector2& vec2)
	{
		return Vector2(x - vec2.x, y - vec2.y);
	}

	Vector2 operator-(const float subtract)
	{
		return Vector2(x - subtract, y - subtract);
	}

	void operator-=(const Vector2& vec2)
	{
		x -= vec2.x;
		y -= vec2.y;
	}

	Vector2 operator*(float scalar)
	{
		return Vector2(x*scalar, y*scalar);
	}

	void operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
	}

	const Vector2 operator/(float divisor)
	{
		return Vector2(x/divisor, y/divisor);
	}

	void operator/=(float scalar)
	{
		x /= scalar;
		y /= scalar;
	}

	bool operator==(const Vector2 &vec2) 
	{
		return x == vec2.x && y == vec2.y;
	}

	bool operator!=(const Vector2& vec2)
	{
		return x != vec2.x || y != vec2.y;
	}

	float distance(const Vector2& vec2 = Vector2())
	{
		//distance between 2 vectors, magnitude of vector if no argument given
		return sqrt((x - vec2.x) * (x - vec2.x) + (y - vec2.y) * (y - vec2.y));
	}

	Vector2 normal()
	{
		//returns normal
		return Vector2(x /this->distance(), y / this->distance());
	}

	void normalize() 
	{
		//makes the vector a normal (normalizes it)
		float d = this->distance();
		x /= d;
		y /= d;

	}

	void rotate(float degrees,Vector2 origin = Vector2()) 
	{
		degrees *= PI_180;
		float c = cosf(degrees);
		float s = sinf(degrees);
		float newX = origin.x + ((x - origin.x) * c - (y - origin.y) * s);
		float newY = origin.y + ((x - origin.x) * s + (y - origin.y) * c);
		x = newX;
		y = newY;
	}

	Vector2 rotated(float degrees) 
	{
		degrees *= PI_180;
		float c = cosf(degrees);
		float s = sinf(degrees);
		float newX = x * c - y * s;
		float newY = x * s + y * c;
		return Vector2(newX,newY);
	}
};

float dot(const Vector2& vec1, const Vector2& vec2) 
{
	return vec1.x * vec2.x + vec1.y * vec2.y;
}

float cross(const Vector2& vec1, const Vector2& vec2) //cross product for 2d vectors returns a scalar
{
	return vec1.x * vec2.y - vec1.y * vec2.x;
}

Vector2 cross(const Vector2& vec1, float a) 
{
	return Vector2(a*vec1.y,-a*vec1.x);
}

Vector2 cross(float a, const Vector2& vec1)
{
	return Vector2(-a * vec1.y, a * vec1.x);
}

float distance(const Vector2& vec1, const Vector2& vec2)
{
	return sqrt((vec1.x - vec2.x) * (vec1.x - vec2.x) + (vec1.y - vec2.y) * (vec1.y - vec2.y));
}

float distanceSqr(const Vector2& vec1, const Vector2& vec2)//returns distance squared (x)^2+(y)^2
{
	return (vec1.x - vec2.x) * (vec1.x - vec2.x) + (vec1.y - vec2.y) * (vec1.y - vec2.y);
}

Vector2 rotate(const Vector2& vec2,float degrees) 
{
	degrees *= PI / 180.0f;
	float c = cosf(degrees);
	float s = sinf(degrees);
	float newX = vec2.x * c - vec2.y * s;
	float newY = vec2.x * s + vec2.y * c;
	return Vector2(newX, newY);
}

Vector2 perpendicular(const Vector2& vec2) 
{
	return Vector2(-vec2.y,vec2.x);
}

float DegtoRad(float angle) 
{
	return angle * (float)PI_180;
}

float RadtoDeg(float angle)
{
	return angle * (float)INVPIx180;
}

bool nearlyEqual(float a, float b) 
{
	return abs(a-b) < VERY_SMALL_VALUE;
}

bool nearlyEqual(Vector2 v1, Vector2 v2)
{
	return distance(v2, v1) < VERY_SMALL_VALUE;
}

template <typename t>
t max(t x, t y) 
{
	return (x > y) ? x : y;
}

template <typename t>
t min(t x, t y)
{
	return (x < y) ? x : y;
}

template <typename t>
t clamp(t mini,t maxi,t value) 
{
	return max(mini,min(maxi,value));
}