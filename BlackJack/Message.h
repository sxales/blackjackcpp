#pragma once

#include <string>

struct Message {
	int timeout=0, tick=0, x, y, delta=0, size;
	int type;
	std::string message;

	Message(std::string _msg, int _type, int _x, int _y, int _size, int _timeout) : message(_msg), type(_type), x(_x), y(_y), size(_size), timeout(_timeout) {}
};