#pragma once

#ifndef RECT_H
#define RECT_H

class Rect {
public:
	int x=0, y=0, width=0, height=0;
	Rect() {}
	Rect(int _x, int _y, int _width, int _height) : x(_x), y(_y), width(_width), height(_height) {}

	bool overlaps(Rect r) {
		//The sides of the rectangles
		int leftA, leftB;
		int rightA, rightB;
		int topA, topB;
		int bottomA, bottomB;

		//Calculate the sides of rect A
		leftA = x;
		rightA = x + width;
		topA = y;
		bottomA = y + height;

		//Calculate the sides of rect B
		leftB = r.x;
		rightB = r.x + r.width;
		topB = r.y;
		bottomB = r.y + r.height;

		//If any of the sides from A are outside of B
		if (bottomA <= topB)
		{
			return false;
		}

		if (topA >= bottomB)
		{
			return false;
		}

		if (rightA <= leftB)
		{
			return false;
		}

		if (leftA >= rightB)
		{
			return false;
		}

		//If none of the sides from A are outside B
		return true;
	}
};

#endif