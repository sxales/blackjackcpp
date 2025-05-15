#pragma once

#include <vector>
#include "Card.h"

class Hand {
private:
	std::vector<Card> cards;
public:
	bool ace = false;

	void reset() {
		if (cards.size() > 0) cards.clear();
		ace = false;
	}

	void add(Card c) {
		if (c.value == 1) ace = true;
		cards.push_back(c);
	}

	int value() {
		int count = 0;
		for (int i = 0; i < cards.size(); i++) {
			count += cards[i].value;
		}
		if (count <= 11 && ace) count += 10;
		return count;
	}

	int size() {
		return cards.size();
	}

	Card get(int i) {
		if (i < cards.size()) return cards[i];
		//else console.log("Out of bounds!");
	}
};