#pragma once

enum Suit { club, diamond, spade, heart };

struct Card {
	Suit suit;
	int face;
	int value;

	Card(Suit _suit, int _face, int _value) : suit(_suit), face(_face), value(_value) {}
};