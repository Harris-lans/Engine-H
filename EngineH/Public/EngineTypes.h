//
// * ENGINE-X
//
// + Types.h
// representations of: 2D vector with floats, 2D vector with integers and RBGA color as four bytes
//

#pragma once
#include "math.h"

//-----------------------------------------------------------------
//-----------------------------------------------------------------

struct exVector2
{
	float x, y;

public:
	void Normalize()
	{
		float magnitude = Magnitude();
		x = x / magnitude;
		y = y / magnitude;
	}

	float Magnitude()
	{
		return (sqrt((x*x) + (y*y)));
	}

	bool operator== (exVector2& pVector)
	{
		return (x == pVector.x && y == pVector.y);
	}

	exVector2 operator* (float pNumber)
	{
		return { this->x * pNumber , this->y * pNumber };
	}

	exVector2 operator- (exVector2& pVector)
	{
		return { this->x - pVector.x , this->y - pVector.y };
	}

	static float DotProduct(exVector2& pVector1, exVector2& pVector2)
	{
		return (pVector1.x * pVector2.x + pVector1.y * pVector2.y);
	}
};


//-----------------------------------------------------------------
//-----------------------------------------------------------------

struct exIntegerVector2
{
	int x, y;
};

//-----------------------------------------------------------------
//-----------------------------------------------------------------

struct exColor
{
	unsigned char mColor[4];

	void SetColor(unsigned char cValue1, unsigned char cValue2, unsigned char cValue3, unsigned char cValue4 = 255)
	{
		mColor[0] = cValue1;
		mColor[1] = cValue2;
		mColor[2] = cValue3;
		mColor[3] = cValue4;
	}
};

struct exColorF
{
	float mColor[4];

	static void ToColorF(exColor& pColor, exColorF& pNormalizedColor)
	{
		for (int i = 0; i < 4; ++i)
		{
			pNormalizedColor.mColor[i] = pColor.mColor[i] / 255;
		}
	}
};