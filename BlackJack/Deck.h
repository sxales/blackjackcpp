#pragma once

#include <vector>
#include <algorithm>
#include "Card.h"

class Deck {
public:
	std::vector<Card> cards;
	int size; //number of decks in the shoe
	int position = 0;

	void init(int _size) {
		size = _size;

		if (cards.size() > 0) cards.clear();

		for (int i = 0; i < size; i++) {
			for (int j = 0; j < 52; j++) {
				Card c = Card(static_cast<Suit>((int)floor(j / 13)), j % 13 + 1, std::min(j % 13 + 1, 10));
				cards.push_back(c);
			}
		}
	}

	void swap(int a, int b) {
		Card temp = cards[a];
		cards[a] = cards[b];
		cards[b] = temp;
	}

	void shuffle() {
		position = 0;

		for (int i = 0; i < size * 52; i++) {
			swap(i, floor(rand() % (size * 52)));
		}
	}

	Card deal() {
		if (position < size * 52) return cards[position++];
		else {
			shuffle();
			return cards[position++];
			//console.log("Deck out of cards!");
		}
	}
};