#include <time.h>
#include "Game.h"
#include "ResourceManager.h"
#include "LWindow.h"
#include "Deck.h"
#include <iostream>

Game::Game() {
	srand(time(NULL));

	_fontsize = (SCREEN_WIDTH * (1 - SCREENRATIO)) / (25);

	_tick = 0;
	_nexttick = DEMO_DELAY;

	_cardwidth = (SCREEN_WIDTH * (1 - SCREENRATIO) - _fontsize) / 5;
	_cardheight = _cardwidth * CARDHEIGHT / CARDWIDTH;
	int vpos = (SCREEN_HEIGHT - _cardheight) / 2;
	int hpos = SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - _cardwidth * 5) / 2;

	int buttonheight = _fontsize * 2;
	int buttonwidth = SCREEN_WIDTH / 7;

	btnincrease = Button("+", SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - _fontsize * 3) / 2, (SCREEN_HEIGHT / 2 - _fontsize * 3.5), _fontsize * 3, _fontsize * 3);
	btndecrease = Button("-", SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - _fontsize * 3) / 2, (SCREEN_HEIGHT / 2 + _fontsize * 0.5), _fontsize * 3, _fontsize * 3);
	btnconfirm = Button("done", SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - buttonwidth * 2) / 2, SCREEN_HEIGHT - buttonheight - _fontsize / 2, buttonwidth * 2, buttonheight);

	btndeal = Button("deal", SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - buttonwidth * 2) / 2, SCREEN_HEIGHT - buttonheight - _fontsize / 2, buttonwidth * 2, buttonheight);

	buttonwidth = (SCREEN_WIDTH * (1 - SCREENRATIO)) / 4;

	btnhit = Button("hit", SCREEN_WIDTH * SCREENRATIO + (buttonwidth * 1), SCREEN_HEIGHT - buttonheight - _fontsize / 2, buttonwidth, buttonheight);
	btnstay = Button("stay", SCREEN_WIDTH * SCREENRATIO + (buttonwidth * 2), SCREEN_HEIGHT - buttonheight - _fontsize / 2, buttonwidth, buttonheight);
	btndoubledown = Button("double", SCREEN_WIDTH * SCREENRATIO + (buttonwidth * 3), SCREEN_HEIGHT - buttonheight - _fontsize / 2, buttonwidth, buttonheight);
	btnsplit = Button("split", SCREEN_WIDTH * SCREENRATIO + (buttonwidth * 0), SCREEN_HEIGHT - buttonheight - _fontsize / 2, buttonwidth, buttonheight);

	btninsuranceyes = Button("yes", SCREEN_WIDTH * SCREENRATIO + (buttonwidth * 1), SCREEN_HEIGHT - buttonheight - _fontsize / 2, buttonwidth, buttonheight);
	btninsuranceno = Button("no", SCREEN_WIDTH * SCREENRATIO + (buttonwidth * 2), SCREEN_HEIGHT - buttonheight - _fontsize / 2, buttonwidth, buttonheight);

	btnquit= Button("X", SCREEN_WIDTH - _fontsize * 2, 0, _fontsize * 2, _fontsize * 2);

	int fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
	int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
	vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2;
	hpos = (SCREEN_WIDTH * SCREENRATIO - dealerwidth) / 2;
	btndealer = Button("", hpos, vpos, dealerwidth, dealerwidth);

	shoe.init(4);

	_state = title;
}

bool Game::load() {
	ResourceManager::getInstance()->addAsset("font", "Data/Textures/font.png");
	ResourceManager::getInstance()->addAsset("cards", "Data/Textures/cards.png");
	ResourceManager::getInstance()->addAsset("dealer", "Data/Textures/dealer_all_small.png");
	ResourceManager::getInstance()->addAsset("tileset", "Data/Textures/tiles.png");
	ResourceManager::getInstance()->addAsset("royalflush", "Data/Textures/BLACKJACK.png");

	return true;
}

bool Game::unload() {
	ResourceManager::getInstance()->removeAsset("font");
	ResourceManager::getInstance()->removeAsset("cards");
	ResourceManager::getInstance()->removeAsset("dealer");
	ResourceManager::getInstance()->removeAsset("tileset");
	ResourceManager::getInstance()->removeAsset("royalflush");

	return true;
}

void Game::render(float interpolation) {
	//Clear screen
	SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 27, 94, 32, 0xFF);
	SDL_RenderClear(LWindow::getInstance()->mRenderer);

	//draw title screen
	if (_state == title) {
		int col = 15;
		int row = 6;

		int adjustedwidth = SCREEN_WIDTH / col;
		int adjustedheight = SCREEN_HEIGHT / row;

		SDL_Rect cardResize = { 0, 0, adjustedwidth, adjustedheight };

		for (int i = 0; i < row * col; i++) {
			SDL_Rect cardTexture;
			if (_tick % 2 == 0) cardTexture = { 13 * CARDWIDTH, CARDHEIGHT * (i % 2), CARDWIDTH, CARDHEIGHT };
			else cardTexture = { 13 * CARDWIDTH, CARDHEIGHT * ((i + 1) % 2), CARDWIDTH, CARDHEIGHT };
			ResourceManager::getInstance()->getAsset("cards")->render(3 + adjustedwidth * (i % col), adjustedheight * (floor(i / col)), &cardTexture, &cardResize);
		}

		int w = SCREEN_HEIGHT / 2 - _fontsize * 2;
		int vpos = (SCREEN_HEIGHT - w) / 2;
		int hpos = (SCREEN_WIDTH - w) / 2;
		//draw dealer
		//drawBox(ctx, 20, hpos, vpos, w, w);
		int c = row * col * 2 / 5;
		SDL_Rect dealerTexture = { DEALERSIZE * 0, 0, DEALERSIZE, DEALERSIZE };
		SDL_Rect dealerResize = { 0, 0, w - 12, w - 12 };
		ResourceManager::getInstance()->getAsset("royalflush")->render(hpos + 5, vpos + 5, &dealerTexture, &dealerResize);

		int height = _fontsize * 2;
		int width = _fontsize * 13;
		vpos = (vpos - _fontsize * 2) / 2;
		hpos = (SCREEN_WIDTH - width) / 2;

		drawBox(20, hpos, vpos - _fontsize / 2, width, height);
		renderText("BlackJack!", (SCREEN_WIDTH - _fontsize * 10) / 2, vpos, ResourceManager::getInstance()->getAsset("font"), super, _fontsize);

		if (_tick % 2 == 0) renderText("Click to start", (SCREEN_WIDTH - _fontsize * 14) / 2, w + (SCREEN_HEIGHT - w) / 2 + vpos, ResourceManager::getInstance()->getAsset("font"), blue, _fontsize);
	}
	else if (_state > title && _state < gameover) btnquit.draw();

	//draw cards
	if (_state >= deal && _state < gameover) {
		if (split) {
			int vpos = (SCREEN_HEIGHT - _cardheight) / 2;
			int offset = SCREEN_WIDTH * SCREENRATIO;
			int playerside = SCREEN_WIDTH - offset;
			playerside /= 2;
			int handwidth = _cardwidth * (hand.size() * 0.2) + (_cardwidth * 0.8);//one full card plus 20% of each other card
			int hpos = offset + ((playerside - handwidth) / 2);
			int fs = _cardwidth / 5;

			for (int i = 0; i < hand.size(); i++) {
				SDL_Rect cardTexture = { (hand.get(i).face - 1) * CARDWIDTH, hand.get(i).suit * CARDHEIGHT, CARDWIDTH, CARDHEIGHT };
				SDL_Rect cardResize = { 0, 0, _cardwidth, _cardheight };
				ResourceManager::getInstance()->getAsset("cards")->render(hpos + (_cardwidth * 0.2) * i, vpos, &cardTexture, &cardResize);
			}

			//draw turn indicator
			if ((_state == hitstay1 || _state == flip) && _tick % 2 == 0) {
				int hpos = offset + ((playerside - fs) / 2);
				renderText("^", hpos, vpos + _cardheight + fs, ResourceManager::getInstance()->getAsset("font"), pink, fs);
			}

			if (outcome < nothing && _tick % 2 == 0) {
				int hpos = offset + ((playerside - fs * 4) / 2);
				switch (outcome) {
				case win:
					renderText("Win!", hpos, vpos + _cardheight + fs, ResourceManager::getInstance()->getAsset("font"), green, fs);
					break;
				case lose:
					renderText("Lose", hpos, vpos + _cardheight + fs, ResourceManager::getInstance()->getAsset("font"), red, fs);
					break;
				case push:
					renderText("Push", hpos, vpos + _cardheight + fs, ResourceManager::getInstance()->getAsset("font"), gray, fs);
					break;
				}
			}
			hpos += playerside;
			for (int i = 0; i < hand2.size(); i++) {
				SDL_Rect cardTexture = { (hand2.get(i).face - 1) * CARDWIDTH, hand2.get(i).suit * CARDHEIGHT, CARDWIDTH, CARDHEIGHT };
				SDL_Rect cardResize = { 0, 0, _cardwidth, _cardheight };
				ResourceManager::getInstance()->getAsset("cards")->render(hpos + (_cardwidth * 0.2) * i, vpos, &cardTexture, &cardResize);
			}

			//draw turn indicator
			if ((_state == hitstay2 || _state == flip2) && _tick % 2 == 0) {
				int hpos = offset + playerside + ((playerside - fs) / 2);
				renderText("^", hpos, vpos + _cardheight + fs, ResourceManager::getInstance()->getAsset("font"), pink, fs);
			}

			if (outcome2 < nothing && _tick % 2 == 0) {
				int hpos = offset + playerside + ((playerside - fs * 4) / 2);
				switch (outcome2) {
				case win:
					renderText("Win!", hpos, vpos + _cardheight + fs, ResourceManager::getInstance()->getAsset("font"), green, fs);
					break;
				case lose:
					renderText("Lose", hpos, vpos + _cardheight + fs, ResourceManager::getInstance()->getAsset("font"), red, fs);
					break;
				case push:
					renderText("Push", hpos, vpos + _cardheight + fs, ResourceManager::getInstance()->getAsset("font"), gray, fs);
					break;
				}
			}
		}
		else {
			int vpos = (SCREEN_HEIGHT - _cardheight) / 2;
			int offset = SCREEN_WIDTH * SCREENRATIO;
			int playerside = SCREEN_WIDTH - offset;
			int handwidth = _cardwidth * (max(hand.size(), 2) * 0.2) + (_cardwidth * 0.8);//one full card plus 20% of each other card
			int hpos = offset + ((playerside - handwidth) / 2);
			int fs = _cardwidth / 5;
			for (int i = 0; i < max(hand.size(),2); i++) {
				SDL_Rect cardTexture = { 13 * CARDWIDTH, 0 * CARDHEIGHT, CARDWIDTH, CARDHEIGHT};
				if (i < hand.size()) cardTexture = { (hand.get(i).face - 1) * CARDWIDTH, hand.get(i).suit * CARDHEIGHT, CARDWIDTH, CARDHEIGHT };
				SDL_Rect cardResize = { 0, 0, _cardwidth, _cardheight };
				ResourceManager::getInstance()->getAsset("cards")->render(hpos + (_cardwidth*0.2)*i, vpos, &cardTexture, &cardResize);
			}

			//draw turn indicator
			if ((_state == hitstay1 || _state == flip) && _tick % 2 == 0) {
				int hpos = offset + ((playerside - fs) / 2);
				renderText("^", hpos, vpos + _cardheight + fs, ResourceManager::getInstance()->getAsset("font"), pink, fs);
			}

			if (outcome < nothing && _tick % 2 == 0) {
				int hpos = offset + ((playerside - fs * 4) / 2);
				switch (outcome) {
				case win:
					renderText("Win!", hpos, vpos + _cardheight + fs, ResourceManager::getInstance()->getAsset("font"), green, fs);
					break;
				case lose:
					renderText("Lose", hpos, vpos + _cardheight + fs, ResourceManager::getInstance()->getAsset("font"), red, fs);
					break;
				case push:
					renderText("Push", hpos, vpos + _cardheight + fs, ResourceManager::getInstance()->getAsset("font"), gray, fs);
					break;
				}
			}
		}
	}

	//draw dealer cards
	if (_state >= deal && _state < dealerturn) {
		//one face down and one face up
		SDL_Rect cardTexture = { 13 * CARDWIDTH, 0 * CARDHEIGHT, CARDWIDTH, CARDHEIGHT };
		SDL_Rect cardResize = { 0, 0, _cardwidth * 0.75, _cardheight * 0.75 };

		int fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
		int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
		int hpos = ((SCREEN_WIDTH * SCREENRATIO) - fs * 8) / 2;
		int vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2 + dealerwidth + fs / 2;

		ResourceManager::getInstance()->getAsset("cards")->render(hpos, vpos, &cardTexture, &cardResize);

		if (dealerhand.size() == 2) cardTexture = { (dealerhand.get(1).face - 1) * CARDWIDTH, dealerhand.get(1).suit * CARDHEIGHT, CARDWIDTH, CARDHEIGHT};

		ResourceManager::getInstance()->getAsset("cards")->render(hpos + (_cardwidth * 0.75) * 0.2, vpos, &cardTexture, &cardResize);

	}
	else if (_state >= dealerturn && _state < gameover) {
		//both face up
		int fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
		int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
		int hpos = ((SCREEN_WIDTH * SCREENRATIO) - fs * 8) / 2;
		int vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2 + dealerwidth + fs / 2;
		for (int i = 0; i < dealerhand.size(); i++) {
			SDL_Rect cardTexture = { (dealerhand.get(i).face - 1) * CARDWIDTH, dealerhand.get(i).suit * CARDHEIGHT, CARDWIDTH, CARDHEIGHT };
			SDL_Rect cardResize = { 0, 0, _cardwidth * 0.75, _cardheight * 0.75 };
			ResourceManager::getInstance()->getAsset("cards")->render(hpos + (_cardwidth * 0.75) * (i * 0.2), vpos, &cardTexture, &cardResize);
		}

		//draw turn indicator
		if (_state == dealerturn && _tick % 2 == 0) {
			int hpos = ((SCREEN_WIDTH * SCREENRATIO) - fs) / 2;
			renderText("^", hpos, vpos + _cardheight * 0.75 + fs, ResourceManager::getInstance()->getAsset("font"), pink, fs);
		}
	}

	//draw buttons
	if (demo && _state < gameover) {
		int buttonheight = _fontsize * 2;
		int buttonwidth = _fontsize * 8;
		if (_tick % 2 == 0) renderText("DEMO", (SCREEN_WIDTH * SCREENRATIO) + (SCREEN_WIDTH * (1 - SCREENRATIO) - buttonwidth) / 2, SCREEN_HEIGHT - buttonheight - _fontsize / 2, ResourceManager::getInstance()->getAsset("font"), super, _fontsize * 2);
	}
	else if (_state == deal) btndeal.draw();
	//else if (_state == keep_or_discard) btndraw.draw();
	else if (_state == setbet) {
		btnconfirm.draw();
		btnincrease.draw();
		btndecrease.draw();
	}
	else if (_state == insurance) {
		btninsuranceno.draw();
		btninsuranceyes.draw();
	}
	else if (_state >= hitstay1 && _state <= hitstay2) {
		btnhit.draw();
		btnstay.draw();
		if (!split && hand.size() == 2 && hand.value() >= 9 && hand.value() <= 11 && bank >= bet) btndoubledown.draw();
		if (!split && hand.size() == 2 && hand.get(0).face == hand.get(1).face && bank >= bet) btnsplit.draw();
	}

	//draw scoreboard
	if (_state >= setbet && _state < gameover) {
		if (displayedbank < bank) displayedbank += round(bet * 0.05);
		else displayedbank = bank;

		//renderText("", x, y, ResourceManager::getInstance()->getAsset("font"), white, fs);

		std::string txt = "";
		if (_state == setbet) txt += to_string(defaultbet);
		else txt += to_string(bet);

		int fs = (SCREEN_WIDTH * (1 - SCREENRATIO)) / (25);
		int vpos = fs / 2;
		int hpos = SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - fs * (17 + txt.length())) / 2;

		drawBox(20, hpos, vpos, fs * (17 + txt.length()), fs * 2);
		renderText("bank", hpos + fs / 2, vpos + fs / 2, ResourceManager::getInstance()->getAsset("font"), white, fs);
		renderText(fill(to_string(displayedbank), 6, "0"), hpos + fs / 2 + fs * 5, vpos + fs / 2, ResourceManager::getInstance()->getAsset("font"), pink, fs);

		renderText("bet", hpos + fs / 2 + fs * 12, vpos + fs / 2, ResourceManager::getInstance()->getAsset("font"), white, fs);
		renderText(txt, hpos + fs / 2 + fs * 16, vpos + fs / 2, ResourceManager::getInstance()->getAsset("font"), pink, fs);

		//draw dealer
		fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
		int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
		vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2;
		hpos = (SCREEN_WIDTH * SCREENRATIO - dealerwidth) / 2;
		//drawBox(20, hpos, vpos, dealerwidth, dealerwidth);
		SDL_Rect outline = { hpos, vpos, dealerwidth, dealerwidth };
		SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 224, 224, 0, 255);
		SDL_RenderFillRect(LWindow::getInstance()->mRenderer, &outline);
		SDL_Rect dealerTexture = { DEALERSIZE * (_dealer), DEALERSIZE * (_level), DEALERSIZE, DEALERSIZE };
		SDL_Rect dealerResize = { 0, 0, dealerwidth - 10, dealerwidth - 10 };
		ResourceManager::getInstance()->getAsset("dealer")->render(hpos + 5, vpos + 5, &dealerTexture, &dealerResize);

		//draw payouts
		hpos = ((SCREEN_WIDTH * SCREENRATIO) - fs * 8) / 2;
		vpos = vpos + dealerwidth + fs / 2;

		/*txt = "bj";
		renderText(txt, hpos + (3 * fs), vpos + fs * 0, ResourceManager::getInstance()->getAsset("font"), yellow, fs);
		txt = "payout";
		renderText(txt, hpos + (1 * fs), vpos + fs * 1, ResourceManager::getInstance()->getAsset("font"), yellow, fs);
		if (bet < 100) txt = "3/2";
		else if (bet < 200) txt = "5/6";
		else txt = "1/1";
		renderText(txt, hpos + (2.5 * fs), vpos + fs * 2.25, ResourceManager::getInstance()->getAsset("font"), yellow, fs);*/

		if (_level < MAXLEVEL) {
			renderText("Affec.", hpos + fs * 1, vpos + fs * 10, ResourceManager::getInstance()->getAsset("font"), yellow, fs);

			int xpbarwidth = ((fs * 8) - 3) * ((double)_currentxp / (double)_nextxp);
			SDL_Rect xpbaroutline = { hpos , vpos + fs * 11, fs * 8, fs };
			SDL_RenderDrawRect(LWindow::getInstance()->mRenderer, &xpbaroutline);

			if (xpbarwidth > 0) {
				SDL_Rect xpbarlight = { hpos + 1, vpos + 1 + (fs * 11), xpbarwidth - 2, fs / 2 };
				SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 255, 255, 255, 255);
				SDL_RenderFillRect(LWindow::getInstance()->mRenderer, &xpbarlight);

				SDL_Rect xpbarshadow = { hpos + 3, vpos + (fs * 11.5), xpbarwidth - 1, fs / 2 - 1 };
				SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 0, 0, 0, 255);
				SDL_RenderFillRect(LWindow::getInstance()->mRenderer, &xpbarshadow);

				SDL_Rect xpbar = { hpos + 2, vpos + 2 + (fs * 11), xpbarwidth - 1, fs - 5 };
				SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 102, 2, 60, 255);
				SDL_RenderFillRect(LWindow::getInstance()->mRenderer, &xpbar);
			}
		}

		if (debug) renderText("debug on", hpos, vpos + fs * 12.5, ResourceManager::getInstance()->getAsset("font"), red, fs);
	}
	else if (_state == gameover) {
		int dealerwidth = SCREEN_HEIGHT / 2 - _fontsize * 2;
		int vpos = (SCREEN_HEIGHT - (dealerwidth + _fontsize * 2.5)) / 2;
		int hpos = (SCREEN_WIDTH - dealerwidth) / 2;
		//draw dealer
		//drawBox(20, hpos, vpos, dealerwidth, dealerwidth);
		SDL_Rect outline = { hpos, vpos, dealerwidth, dealerwidth };
		SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 224, 224, 0, 255);
		SDL_RenderFillRect(LWindow::getInstance()->mRenderer, &outline);
		SDL_Rect dealerTexture = { DEALERSIZE * (_dealer), DEALERSIZE * (_level), DEALERSIZE, DEALERSIZE };
		SDL_Rect dealerResize = { 0, 0, dealerwidth - 10, dealerwidth - 10 };
		ResourceManager::getInstance()->getAsset("dealer")->render(hpos + 5, vpos + 5, &dealerTexture, &dealerResize);

		renderText("Game Over!", (SCREEN_WIDTH - _fontsize * 10) / 2, vpos / 2, ResourceManager::getInstance()->getAsset("font"), super, _fontsize);

		drawBox(20, (SCREEN_WIDTH - _fontsize * 13) / 2 - _fontsize / 2, vpos + dealerwidth + _fontsize / 2, _fontsize * 14, _fontsize * 2);
		renderText("See you again", (SCREEN_WIDTH - _fontsize * 13) / 2, vpos + dealerwidth + _fontsize, ResourceManager::getInstance()->getAsset("font"), logo, _fontsize);

		std::string timer = "" + to_string((int)round((_nexttick - _tick) / 3 + 1));

		renderText(timer, (SCREEN_WIDTH - _fontsize * timer.length()) / 2, SCREEN_HEIGHT - (vpos / 2), ResourceManager::getInstance()->getAsset("font"), yellow, _fontsize);
	}

	//draw dealer dialog
	if (_state == setbet) {
		int fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
		int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
		int vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2 + _fontsize / 2;
		int hpos = (SCREEN_WIDTH * SCREENRATIO);

		drawBox(20, hpos - _fontsize / 2, vpos - _fontsize / 2, _fontsize * 12, _fontsize * 2);
		renderText("Select bet!", hpos, vpos, ResourceManager::getInstance()->getAsset("font"), logo, _fontsize);
	}
	else if (_state == insurance) {
		int fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
		int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
		int vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2 + _fontsize / 2;
		int hpos = (SCREEN_WIDTH * SCREENRATIO);

		drawBox(20, hpos - _fontsize / 2, vpos - _fontsize / 2, _fontsize * 11, _fontsize * 2);
		renderText("Insurance?", hpos, vpos, ResourceManager::getInstance()->getAsset("font"), logo, _fontsize);
	}

	//draw messages
	for (int i = 0; i < _messages.size(); i++) {
		_messages[i].y += _messages[i].delta;
		drawBox(20, _messages[i].x - _messages[i].size / 2, _messages[i].y - _messages[i].size / 2, _messages[i].message.length() * _messages[i].size + _messages[i].size, _messages[i].size * 2);
		renderText(_messages[i].message, _messages[i].x, _messages[i].y, ResourceManager::getInstance()->getAsset("font"), _messages[i].type, _messages[i].size);
	}

	//Update screen
	SDL_RenderPresent(LWindow::getInstance()->mRenderer);
}

void Game::update() {
	++_tick;

	if (demo) {
		if (!debug) --demo_timeout;
		if (demo_timeout < 0) {
			outcome = nothing;
			outcome2 = nothing;
			bank = 500;
			defaultbet = 50;
			bet = defaultbet;
			bet2 = 0;
			displayedbank = bank;
			_nexttick = _tick + DEMO_DELAY;
			demo = false;
			_dealer = 0;
			_level = 0;
			_currentxp = 0;
			_nextxp = 10;
			cout << "Demo ended." << endl;
			_messages.clear();
			_state = title;
		}
	}

	if (_state == title && _nexttick < _tick) {
		demo = true;
		demo_timeout = 150;
		bank = 500;
		defaultbet = 50;
		bet = defaultbet;
		displayedbank = bank;
		_dealer = rand() % MAXDEALER;
		_nexttick = _tick + 5;
		cout << "Demo started." <<  endl;
		_state = deal;
	}
	else if (demo && _state == deal && _nexttick < _tick) {
		bank -= bet;

		hand.reset();
		hand2.reset();
		dealerhand.reset();

		shoe.shuffle();

		hand.add(shoe.deal());
		dealerhand.add(shoe.deal());
		hand.add(shoe.deal());
		dealerhand.add(shoe.deal());

		if (hand.value() == 21) {
			//blackjack
			writeDialog("Player Blackjack!");

			//_nexttick = _tick + 5;
			_state = score;
		}
		else if (dealerhand.value() == 21) {
			//blackjack
			writeDialog("Dealer Blackjack!");

			//_nexttick = _tick + 5;
			_state = score;
		}
		else {
			_nexttick = _tick + 5;
			_state = flip;
		}
	}
	else if (demo && _state == flip && _nexttick < _tick) {
		Action action = get_strategy(hand, dealerhand);

		if (split && action == dbldwn) action = hit;

		switch (action) {
		case hit:
			hand.add(shoe.deal());
			if (hand.value() > 21) {
				//bust
				writeDialog("Player Bust!");
				if (split) {
					hand2.add(shoe.deal());
					if (hand2.value() == 21) {
						//blackjack
						writeDialog("Player Blackjack!");
						_state = dealerturn;
					}
					else {
						hand2.add(shoe.deal());
						if (hand2.value() == 21) {
							//blackjack
							writeDialog("Player Blackjack!");
							_state = dealerturn;
						}
						else _state = flip2;
					}
				}
				else {
					_nexttick = _tick + 5;
					_state = score;
				}
			}
			_nexttick = _tick + 5;
			break;
		case stay:
			_nexttick = _tick + 5;
			if (split) {
				hand2.add(shoe.deal());
				if (hand2.value() == 21) {
					//blackjack
					writeDialog("Player Blackjack!");
					_state = dealerturn;
				}
				else _state = flip2;
			}
			else _state = dealerturn;
			break;
		case dbldwn:
			hand.add(shoe.deal());
			bank -= bet;
			bet *= 2;
			_nexttick = _tick + 5;
			_state = dealerturn;
			break;
		case splt: {
			split = true;
			bet2 = bet;
			bank -= bet2;
			if (hand.get(0).face == 1) aces = true;
			else aces = false;

			hand2.reset();
			hand2.add(hand.get(1));
			Card temp = hand.get(0);
			hand.reset();
			hand.add(temp);
			hand.add(shoe.deal());

			if (hand.value() == 21) {
				//blackjack
				writeDialog("Player Blackjack!");
				//bank += bet2*2.5;
				hand2.add(shoe.deal());

				if (hand2.value() == 21) {
					writeDialog("Player Blackjack!");
					//_nexttick = _tick + 5;
					_state = score;
				}
				else if (aces) _state = dealerturn;
				else _state = flip2;
			}
			else if (aces) {
				hand2.add(shoe.deal());

				if (hand2.value() == 21) {
					writeDialog("Player Blackjack!");
					//_nexttick = _tick + 5;
					_state = score;
				}
				_state = dealerturn;
			}
			break;
		}
		}
	}
	else if (demo && _state == flip2 && _nexttick < _tick) {
		Action action = get_strategy(hand2, dealerhand);

		if (split && action == dbldwn) action = hit;

		switch (action) {
		case hit:
			hand2.add(shoe.deal());
			if (hand2.value() > 21) {
				//bust
				writeDialog("Player Bust!");

				//_nexttick = _tick + 5;
				_state = score;
			}
			_nexttick = _tick + 5;
			break;
		case stay:
			_nexttick = _tick + 5;
			_state = dealerturn;
			break;
		}
	}
	else if (_state == dealerturn && _nexttick < _tick) {
		_nexttick = _tick + 5;//delay between cards
		if (dealerhand.value() == 21 && dealerhand.size() == 2) {
			//blackjack
			writeDialog("Dealer Blackjack!");
			_state = score;
		}
		else if (((!split && hand.value() < 22) || (split && (hand.value() < 22 || hand2.value() < 22))) && dealerhand.value() < 17) {
			dealerhand.add(shoe.deal());

			std::cout << "Dealer takes a card. Dealer shows: ";
			for (int i = 0;i < dealerhand.size();i++) std::cout << translate_card(dealerhand.get(i));
			std::cout << "(" << to_string(dealerhand.value()) << ")" << endl;

			if (dealerhand.value() > 21) {
				//bust
				writeDialog("Dealer Bust!");
				_state = score;
			}
		}
		else {
			std::cout << "Dealer stays. Dealer shows: ";
			for (int i = 0;i < dealerhand.size();i++) std::cout << translate_card(dealerhand.get(i));
			std::cout << "(" << to_string(dealerhand.value()) << ")" << endl;
			_state = score;
		}
	}
	else if (_state == score) {
		_nexttick = _tick + SCORE_DELAY - 2;
		outcome = evaluate(hand, dealerhand);
		if (split) outcome2 = evaluate(hand2, dealerhand);
		
		int fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
		int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
		int vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2 + _fontsize / 2;
		int hpos = (SCREEN_WIDTH * SCREENRATIO);

		if (outcome == win) {
			if (hand.value() == 21 && hand.size() == 2) bank += bet * 2.5;
			else bank += bet * 2;
			std::cout << "Player wins! Current bank balance: " << to_string(bank) << endl;
		}
		else if (outcome == push) {
			bank += bet;
			std::cout << "Player pushes. Current bank balance: " << to_string(bank) << endl;
		}
		else if (outcome == lose) {
			if (hasinsurance && dealerhand.value() == 21 && dealerhand.size() == 2)	bank += bet;
			std::cout << "Player loses. Current bank balance: " << to_string(bank) << endl;
		}

		if (split) {
			if (outcome2 == win) {
				if (hand2.value() == 21 && hand2.size() == 2) bank += bet2 * 2.5;
				else bank += bet2 * 2;
				std::cout << "Player hand 2 wins! Current bank balance: " << to_string(bank) << endl;
			}
			else if (outcome2 == push) {
				bank += bet2;
				std::cout << "Player hand 2 pushes. Current bank balance: " << to_string(bank) << endl;
			}
			else if (outcome2 == lose) {
				std::cout << "Player loses. Current bank balance: " << to_string(bank) << endl;
			}
		}

		if (_level < MAXLEVEL) _currentxp += 5 * (bet / 250.0);

		_state = reset;
	}
	else if (_state == reset && _nexttick < _tick) {
		//std::fill(std::begin(hold), std::end(hold), false);
		outcome = nothing;
		outcome2 = nothing;
		hasinsurance = false;
		split = false;
		bet = defaultbet;
		bet2 = 0;

		hand.reset();
		hand2.reset();
		dealerhand.reset();

		if (_currentxp >= _nextxp) {
			++_level;
			_currentxp = 0;
			_nextxp *= 2;
		}

		if (bank < bet) {
			_nexttick = _tick + 25;
			_currentxp = 0;
			std::cout << "Game Over!" << endl;
			_state = gameover;
		}
		else {
			_nexttick = _tick + 5;
			_state = deal;
		}
	}
	else if (_state == gameover && _nexttick < _tick) {
		bank = 500;
		defaultbet = 50;
		bet = defaultbet;
		bet2 = 0;
		displayedbank = bank;
		_nexttick = _tick + DEMO_DELAY;
		demo = false;
		_dealer = 0;
		_level = 0;
		_currentxp = 0;
		_nextxp = 10;
		//demo_timeout = 300;
		_state = title;
	}

	//update messages
	_messages.erase(std::remove_if(_messages.begin(), _messages.end(), [](Message& m) {
		return (++m.tick >= m.timeout);
	}), _messages.end());
}

void Game::handleEvents(SDL_Event& e) {
	if (e.type == SDL_MOUSEMOTION) {
		btndeal.check(e.motion.x, e.motion.y);
		btnconfirm.check(e.motion.x, e.motion.y);
		btnincrease.check(e.motion.x, e.motion.y);
		btndecrease.check(e.motion.x, e.motion.y);
		btnquit.check(e.motion.x, e.motion.y);
		btnhit.check(e.motion.x, e.motion.y);
		btnstay.check(e.motion.x, e.motion.y);
		btndoubledown.check(e.motion.x, e.motion.y);
		btnsplit.check(e.motion.x, e.motion.y);
		btninsuranceyes.check(e.motion.x, e.motion.y);
		btninsuranceno.check(e.motion.x, e.motion.y);
	}
	else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
		if (demo) demo_timeout = -1;
		else if (_state == title) {
			_state = setbet;
			_dealer = rand() % MAXDEALER;
			//std::fill(std::begin(hold), std::end(hold), false);
		}
		else if (btnquit.check(e.motion.x, e.motion.y)) {
			//std::fill(std::begin(hold), std::end(hold), false);
			std::cout << "Player quit!" << endl;
			_messages.clear();
			bank = 500;
			defaultbet = 50;
			bet = defaultbet;
			displayedbank = bank;
			_nexttick = _tick + DEMO_DELAY;
			demo = false;
			_level = 0;
			_currentxp = 0;
			_nextxp = 10;
			outcome = nothing;
			//demo = false;
			_state = title;
		}
		else if (_state == setbet) {
			if (btnconfirm.check(e.motion.x, e.motion.y)) {
				bet = defaultbet;
				_state = deal;
			}
			else if (btnincrease.check(e.motion.x, e.motion.y)) {
				if (defaultbet < 250) defaultbet += 50;
			}
			else if (btndecrease.check(e.motion.x, e.motion.y)) {
				if (defaultbet > 50) defaultbet -= 50;
			}
			else if (btndealer.check(e.motion.x, e.motion.y)) {
				_dealer = (_dealer + 1) % MAXDEALER;
			}
		}
		else if (_state == deal && btndeal.check(e.motion.x, e.motion.y)) {
			bank -= bet;
			std::cout << "Player paid " << to_string(bet) << ", bank balance is now " << to_string(bank) << endl;

			hand.reset();
			hand2.reset();
			dealerhand.reset();

			shoe.shuffle();
			std::cout << "Deck shuffled!" << endl;
			
			hand.add(shoe.deal());
			dealerhand.add(shoe.deal());
			hand.add(shoe.deal());
			dealerhand.add(shoe.deal());

			std::cout << "Player shows: " << translate_card(hand.get(0)) << translate_card(hand.get(1)) << "(" << to_string(hand.value()) << "), Dealer shows: " << translate_card(dealerhand.get(1)) <<  endl;

			if (dealerhand.get(1).face == 1) {
				_state = insurance;
			}
			else if (hand.value() == 21) {
				//blackjack
				writeDialog("Player Blackjack!");

				_nexttick = _tick + 5;
				_state = score;
			}
			else if (dealerhand.value() == 21) {
				//blackjack
				writeDialog("Dealer Blackjack!");

				_nexttick = _tick + 5;
				_state = score;
			}
			else _state = hitstay1;
		}
		else if (_state == insurance) {
			if (btninsuranceyes.check(e.motion.x, e.motion.y)) {
				hasinsurance = true;
				bet = round(bet / 2);

				std::cout << "Player surrends half of bet to buy insurance. Current bet: " << to_string(bet) << endl;

				if (dealerhand.value() == 21 && dealerhand.size() == 2) {
					if (hand.value() == 21 && hand.size() == 2) {
						_state = score;
					}
					else {
						//bank += bet * 2;
						writeDialog("Dealer Blackjack!");
						_nexttick = _tick + 5;
						_state = score;
					}
				}
				else {
					if (hand.value() == 21 && hand.size() == 2) {
						writeDialog("Player Blackjack!");
						_nexttick = _tick + 5;
						_state = score;
					}
					else _state = hitstay1;
				}
			}
			else if (btninsuranceno.check(e.motion.x, e.motion.y)) {
				hasinsurance = false;

				std::cout << "Player waives insurance." << endl;

				if (dealerhand.value() == 21 && dealerhand.size() == 2) {
					if (hand.value() == 21 && hand.size() == 2) {
						_state = score;
					}
					else {
						writeDialog("Dealer Blackjack!");
						_nexttick = _tick + 5;
						_state = score;
					}
				}
				else {
					if (hand.value() == 21 && hand.size() == 2) {
						writeDialog("Player Blackjack!");
						_nexttick = _tick + 5;
						//bank += bet2*2.5;
						_state = score;
					}
					else _state = hitstay1;
				}
			}
		}
		else if (_state == hitstay1) {
			if (btnstay.check(e.motion.x, e.motion.y)) {
				std::cout << "Player stays." << endl;
				if (split) {
					hand2.add(shoe.deal());

					std::cout << "Player hand 2 shows: ";
					for (int i = 0;i < hand2.size();i++) std::cout << translate_card(hand2.get(i));
					std::cout << "(" << to_string(hand2.value()) << ")" << endl;

					if (hand2.value() == 21) {
						//blackjack
						writeDialog("Player Blackjack!");
						_state = dealerturn;
					}
					else _state = hitstay2;
				}
				else {
					_nexttick = _tick + 5;
					_state = dealerturn;
				}
			}
			else if (btnhit.check(e.motion.x, e.motion.y)) {
				hand.add(shoe.deal());

				std::cout << "Player takes a card. Player shows: ";
				for (int i = 0;i < hand.size();i++) std::cout << translate_card(hand.get(i));
				std::cout << "(" << to_string(hand.value()) << ")" << endl;

				if (hand.value() > 21) {
					//bust
					writeDialog("Player Bust!");

					if (split) {
						hand2.add(shoe.deal());

						std::cout << "Player hand 2 shows: ";
						for (int i = 0;i < hand2.size();i++) std::cout << translate_card(hand2.get(i));
						std::cout << "(" << to_string(hand2.value()) << ")" << endl;

						if (hand2.value() == 21) {
							//blackjack
							writeDialog("Player Blackjack!");
							_state = dealerturn;
						}
						else _state = hitstay2;
					}
					else {
						//_nexttick = _tick + 5;
						_state = score;
					}
				}
			}
			else if (!split && hand.value() >= 9 && hand.value() <= 11 && btndoubledown.check(e.motion.x, e.motion.y) && bank >= bet) {
				bank -= bet;
				bet *= 2;

				std::cout << "Player doubles down. Current bet: " << to_string(bet) << endl;

				hand.add(shoe.deal());

				std::cout << "Player takes a card. Player shows: ";
				for (int i = 0;i < hand.size();i++) std::cout << translate_card(hand.get(i));
				std::cout << "(" << to_string(hand.value()) << ")" << endl;

				_nexttick = _tick + 5;

				_state = dealerturn;
			}
			else if (!split && hand.size() == 2 && hand.get(0).face == hand.get(1).face && btnsplit.check(e.motion.x, e.motion.y) && bank >= bet) {
				split = true;
				bet2 = bet;
				bank -= bet2;

				std::cout << "Player splits, bank balance is now " << to_string(bank) << endl;

				//are we splitting aces
				if (hand.get(0).face == 1) aces = true;
				else aces = false;

				hand2.reset();
				hand2.add(hand.get(1));
				Card temp = hand.get(0);
				hand.reset();
				hand.add(temp);
				hand.add(shoe.deal());

				std::cout << "Player hand 1 shows: ";
				for (int i = 0;i < hand.size();i++) std::cout << translate_card(hand.get(i));
				std::cout << "(" << to_string(hand.value()) << ")" << endl;

				std::cout << "Player hand 2 shows: ";
				for (int i = 0;i < hand2.size();i++) std::cout << translate_card(hand2.get(i));
				std::cout << "(" << to_string(hand2.value()) << ")" << endl;

				if (hand.value() == 21) {
					//blackjack
					writeDialog("Player Blackjack!");
					//bank += bet2*2.5;
					hand2.add(shoe.deal());

					std::cout << "Player hand 2 shows: ";
					for (int i = 0;i < hand2.size();i++) std::cout << translate_card(hand2.get(i));
					std::cout << "(" << to_string(hand2.value()) << ")" << endl;

					if (hand2.value() == 21) {
						writeDialog("Player Blackjack!");
						_nexttick = _tick + 5;
						_state = score;
					}
					else if (aces) _state = dealerturn;
					else _state = hitstay2;
				}
				else if (aces) {
					hand2.add(shoe.deal());

					std::cout << "Player hand 2 shows: ";
					for (int i = 0;i < hand2.size();i++) std::cout << translate_card(hand2.get(i));
					std::cout << "(" << to_string(hand2.value()) << ")" << endl;

					if (hand2.value() == 21) {
						writeDialog("Player Blackjack!");
						_nexttick = _tick + 5;
						_state = score;
					}
					_state = dealerturn;
				}
			}
			else if (btndealer.check(e.motion.x, e.motion.y)) {
				int fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
				int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
				int vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2 + _fontsize / 2;
				int hpos = (SCREEN_WIDTH * SCREENRATIO);

				Action action = get_strategy(hand, dealerhand);

				std::string txt = translate_action(action);
				_messages.push_back(Message(txt, logo, hpos, vpos, _fontsize, 7));
			}
		}
		else if (_state == hitstay2) {
			if (btnstay.check(e.motion.x, e.motion.y)) {
				std::cout << "Player stays." << endl;
				_nexttick = _tick + 5;
				_state = dealerturn;
			}
			else if (btnhit.check(e.motion.x, e.motion.y)) {
				hand2.add(shoe.deal());

				std::cout << "Player takes a card. Player hand 2 shows: ";
				for (int i = 0;i < hand2.size();i++) std::cout << translate_card(hand2.get(i));
				std::cout << "(" << to_string(hand2.value()) << ")" << endl;

				if (hand2.value() > 21) {
					//bust
					writeDialog("Player Bust!");
					//_nexttick = _tick + 5;
					_state = score;
				}
			}
			else if (btndealer.check(e.motion.x, e.motion.y)) {
				int fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
				int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
				int vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2 + _fontsize / 2;
				int hpos = (SCREEN_WIDTH * SCREENRATIO);

				Action action = get_strategy(hand2, dealerhand);

				std::string txt = translate_action(action);
				_messages.push_back(Message(txt, logo, hpos, vpos, _fontsize, 7));
			}
		}
	}
	else if (e.type == SDL_KEYUP) {
		switch (e.key.keysym.sym) {
		case SDLK_d:
			debug = !debug;
			cout << "Debug: " << to_string(debug) << endl;
			break;
		}
		if (debug) {
			switch (e.key.keysym.sym) {
			case SDLK_UP:
				if (bet < defaultbet * 5) bet += defaultbet;
				break;
			case SDLK_DOWN:
				if (bet > defaultbet) bet -= defaultbet;
				break;
			case SDLK_m:
				bank += 500;
				break;
			case SDLK_n:
				_dealer = (_dealer + 1) % MAXDEALER;
				break;
			}
		}
	}
}

#define FONT_SIZE 40
void Game::renderText(std::string message, int x, int y, LTexture* font, int subtype, int size) {
	for (int i = 0; i < message.length(); i++) {
		int ch = message.at(i);
		SDL_Rect spriteTexture;
		if (ch < 126 && ch >= 32) {
			//select sprite texture
			spriteTexture = { (ch - 32) * FONT_SIZE, subtype * FONT_SIZE, FONT_SIZE, FONT_SIZE };
		}
		else {
			//draw question mark
			//select sprite texture
			spriteTexture = { 41 * FONT_SIZE,  subtype * FONT_SIZE, FONT_SIZE, FONT_SIZE };
		}
		//scale to on screen size
		SDL_Rect rescale = SDL_Rect{ 0, 0, size, size };
		//renter the actor
		font->render(x + (i * size), y, &spriteTexture, &rescale, 0);
	}
}

void Game::drawBox(int color, int x, int y, int width, int height) {
	SDL_Rect texture = { 64 * (color % 8), 64 * floor(color / 8), 16, 16 };//top left corner
	SDL_Rect resize = { 0, 0, 16, 16 };
	ResourceManager::getInstance()->getAsset("tileset")->render(x, y, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//top left corner
	ResourceManager::getInstance()->getAsset("tileset")->render(x, y+height - 16, &texture, &resize, 0.0, NULL, SDL_FLIP_VERTICAL);//bottom left 
	ResourceManager::getInstance()->getAsset("tileset")->render(x+width - 16, y, &texture, &resize, 0.0, NULL, SDL_FLIP_HORIZONTAL);//top right corner
	ResourceManager::getInstance()->getAsset("tileset")->render(x+width - 16, y+height - 16, &texture, &resize, 0.0, NULL, SDL_RendererFlip(SDL_FLIP_VERTICAL|SDL_FLIP_HORIZONTAL));//bottom right corner

	texture = { 64 * (color % 8) + 16, 64 * (int)floor(color / 8), 16, 16 };
	resize = { 0, 0, width - 32, 16 };
	ResourceManager::getInstance()->getAsset("tileset")->render(x + 16, y, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//top
	ResourceManager::getInstance()->getAsset("tileset")->render(x + 16, y + height - 16, &texture, &resize, 0.0, NULL, SDL_FLIP_VERTICAL);//bottom
	
	texture = { 64 * (color % 8), 64 * (int)floor(color / 8) + 16, 16, 16 };
	resize = { 0, 0, 16, height - 32 };
	ResourceManager::getInstance()->getAsset("tileset")->render(x, y + 16, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//left
	ResourceManager::getInstance()->getAsset("tileset")->render(x + width - 16, y + 16, &texture, &resize, 0.0, NULL, SDL_FLIP_HORIZONTAL);//right

	texture = { 64 * (color % 8) + 16, 64 * (int)floor(color / 8) + 16, 16, 16 };
	resize = { 0, 0, width - 32, height - 32 };
	ResourceManager::getInstance()->getAsset("tileset")->render(x + 16, y + 16, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//fill
}

std::string Game::translate_outcome(Outcome outcome) {
	std::string txt = "Error!";
	switch (outcome) {
	case win: 
		txt = "Win";
		break;
	case lose:
		txt = "Lose";
		break;
	case push:
		txt = "Push";
		break;
	}
	return txt;
}

std::string Game::translate_card(Card c) {
	return translate_face(c.face) + translate_suit(c.suit);
}

std::string Game::translate_suit(int s) {
	std::string txt = "Error";
	switch (s) {
	case 0:
		txt = "♣";
		break;
	case 1:
		txt = "♦";
		break;
	case 2:
		txt = "♠";
		break;
	case 3:
		txt = "♥";
		break;
	}
	return txt;
}

std::string Game::translate_face(int f) {
	std::string txt = "Error";
	switch (f) {
	case 1:
		txt = "A";
		break;
	case 11:
		txt = "J";
		break;
	case 12:
		txt = "Q";
		break;
	case 13:
		txt = "K";
		break;
	default:
		txt = to_string(f);
		break;
	}
	return txt;
}

std::string Game::translate_action(Action a) {
	std::string txt = "Error";
	switch (a) {
	case hit:
		txt = "Hit";
		break;
	case stay:
		txt = "Stay";
		break;
	case dbldwn:
		txt = "Double Down";
		break;
	case splt:
		txt = "Split";
		break;
	}
	return txt;
}

Action Game::get_strategy(Hand p, Hand d) {
	//check for split
	if (!split && p.get(0).face == p.get(1).face) {
		//split table
		if (split_table[p.get(0).value - 1][d.get(1).value - 1]) return splt;
	}

	//check for ace
	if (p.ace && p.size() == 2) {
		//soft table
		int val = max(p.get(0).value - 1, p.get(1).value - 1);

		return soft_table[val - 1][d.get(1).value - 1];
	}

	//hard table
	Action a = hard_table[p.value() - 1][d.get(1).value - 1];
	if (p.size() > 2 && a == dbldwn) a = hit;//can only doubledown on first hit

	return a;
}

Outcome Game::evaluate(Hand p, Hand d) {
	if (p.value() > 21) {
		//player bust
		return lose;
	}
	else if (d.value() > 21) {
		//dealer bust
		return win;
	}
	else if (p.value() > d.value()) {
		return win;
	}
	else if (p.value() == d.value()) {
		if (p.value() == 21 && p.size() == 2 && d.size() > 2) return win; //player blackjack beats dealer 21
		else if (d.value() == 21 && d.size() == 2 && p.size() > 2) return lose; //dealer blackjack beats player 21
		else return push;
	}
	else {
		return lose;
	}

	return nothing;
}

std::string Game::fill(std::string txt, int n, std::string pad) {
	std::string s = "" + txt;
	while (s.length() < n) {
		s = pad + s;
	}
	return s;
};

void Game::writeDialog(std::string txt) {
	int fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
	int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
	int vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2 + _fontsize / 2;
	int hpos = (SCREEN_WIDTH * SCREENRATIO);

	std::cout << txt << endl;

	_messages.push_back(Message(txt, logo, hpos, vpos, _fontsize, SCORE_DELAY));
}