#pragma once

#include"SeeAisa.h"

class ColorRGB
{
public:
	float R, G, B;

	ColorRGB() { R = 0.f; G = 0.f; B = 0.f; }
	ColorRGB(float a, float b, float c) { R = a; G = b; B = c; }

	ColorRGB &operator+=(const ColorRGB &color)
	{
		R += color.R; G += color.G; B += color.B;
		return *this;
	}

	ColorRGB operator+(const ColorRGB color) const {
		return ColorRGB(R + color.R, G + color.G, B + color.B);
	}

	ColorRGB operator*(const float f) const
	{
		return ColorRGB(R*f, G*f, B*f);
	}

	ColorRGB operator*(const ColorRGB color)const {
		return ColorRGB(R*color.R, G*color.G, B*color.B);
	}

	void Clear() { R = 0.f; G = 0.f; B = 0.f; }

};
