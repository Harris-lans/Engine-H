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
	exVector2() = default;

	exVector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

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

	static void ToColorF(const exColor& pColor, exColorF& pNormalizedColor)
	{
		for (int i = 0; i < 4; ++i)
		{
			pNormalizedColor.mColor[i] = pColor.mColor[i] / 255.0f;
		}
	}
};

struct exMatrix4
{
public:
	exMatrix4() = default;
	~exMatrix4() = default;

	float* ToFloatPtr()
	{
		return &m11;
	}

	const float* ToFloatPtr() const
	{
		return &m11;
	}

	static void exOrthographicProjectionMatrix(exMatrix4* pOut, float fWidth, float fHeight, float fNearPlane, float fFarPlane)
	{
		// http://msdn.microsoft.com/en-us/library/windows/desktop/dd373965%28v=vs.85%29.aspx
		// http://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/orthographic-projection-matrix
		// https://blog.demofox.org/2017/03/31/orthogonal-projection-matrix-plainly-explained/

		pOut->m11 = 2.0f / fWidth;
		pOut->m12 = 0.0f;
		pOut->m13 = 0.0f;
		pOut->m14 = 0.0f;

		pOut->m21 = 0.0f;
		pOut->m22 = 2.0f / -fHeight;
		pOut->m23 = 0.0f;
		pOut->m24 = 0.0f;

		pOut->m31 = 0.0f;
		pOut->m32 = 0.0f;
		pOut->m33 = -2.0f / (fFarPlane - fNearPlane);
		pOut->m34 = 0.0f;

		pOut->m41 = -1.0f;
		pOut->m42 = 1.0f;
		pOut->m43 = -((fFarPlane + fNearPlane) / (fFarPlane - fNearPlane));
		pOut->m44 = 1.0f;
	}

	static void exMakeTranslationMatrix(exMatrix4* pOut, const exVector2& v2Position)
	{
		pOut->m11 = 1.0f;
		pOut->m22 = 1.0f;
		pOut->m33 = 1.0f;
		pOut->m44 = 1.0f;

		pOut->m12 = pOut->m13 = pOut->m14 = 0.0f;
		pOut->m21 = pOut->m23 = pOut->m24 = 0.0f;
		pOut->m31 = pOut->m32 = pOut->m34 = 0.0f;

		pOut->m41 = v2Position.x;	// identity matrix apart from this translation
		pOut->m42 = v2Position.y;
		pOut->m43 = 0.0f;
	}

public:
	float		m11, m12, m13, m14;
	float		m21, m22, m23, m24;
	float		m31, m32, m33, m34;
	float		m41, m42, m43, m44;


};