#pragma once
#include "raylib.h"

class Worker;

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
	bool reserved = false;
	//Worker* reservedBy;

	Pickup() {}
	Pickup(Capital::ECapitalType type, Vector2 startPos);
	~Pickup() {}

	void Render();
};

