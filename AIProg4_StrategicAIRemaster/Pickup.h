#pragma once
#include "raylib.h"

namespace Capital
{
	enum ECapitalType;
}

class Pickup
{
public:
	Capital::ECapitalType type;
	Vector2 position;
	Vector2 size;
	Color color;

	Pickup() {}
	Pickup(Capital::ECapitalType type, Vector2 startPos);
	~Pickup() {}

	void Render();
};

