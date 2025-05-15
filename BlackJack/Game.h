#pragma once

#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <string>
#include <vector>
#include "LTexture.h"
#include "Scene.h"
#include "Message.h"
#include "Deck.h"
#include "Hand.h"
#include "Button.h"

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

enum State { title, setbet, deal, flip, flip2, insurance, hitstay1, hitstay2, dealerturn, score, reset, gameover };
enum Outcome { win, lose, push, nothing };
enum FontColor { logo, white, super, red, green, blue, yellow, pink, gray };
enum Action { hit, stay, dbldwn, splt };

class Game : public Scene {
private:
	const bool split_table[10][10] = { 
		//A			2		3		4		5		6		7		8		9		10
		{ true,		true,	true,	true,	true,	true,	true,	true,	true,	true }, //A
		{ false,	false,	false,	true,	true,	true,	true,	false,	false,	false}, //2
		{ false,	false,	false,	true,	true,	true,	true,	false,	false,	false}, //3
		{ false,	false,	false,	false,	false,	false,	false,	false,	false,	false}, //4
		{ false,	false,	false,	false,	false,	false,	false,	false,	false,	false}, //5
		{ false,	false,	true,	true,	true,	true,	false,	false,	false,	false }, //6
		{ false,	true,	true,	true,	true,	true,	true,	false,	false,	false }, //7
		{ true,		true,	true,	true,	true,	true,	true,	true,	true,	true }, //8
		{ true,		true,	true,	true,	true,	true,	false,	true,	false,	false }, //9
		{ false,	false,	false,	false,	false,	false,	false,	false,	false,	false} //10
	};
	const Action hard_table[21][10] = {
		//A			2		3		4		5		6		7		8		9		10
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //1
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //2
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //3
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //4
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //5
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //6
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //7
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //8
		{ hit,		hit,	dbldwn,	dbldwn,	dbldwn,	dbldwn,	hit,	hit,	hit,	hit }, //9
		{ hit,		dbldwn,	dbldwn,	dbldwn,	dbldwn,	dbldwn,	dbldwn,	dbldwn,	dbldwn,	hit }, //10
		{ dbldwn,	dbldwn,	dbldwn,	dbldwn,	dbldwn,	dbldwn,	dbldwn,	dbldwn,	dbldwn,	dbldwn }, //11
		{ hit,		hit,	hit,	stay,	stay,	stay,	hit,	hit,	hit,	hit }, //12
		{ hit,		stay,	stay,	stay,	stay,	stay,	hit,	hit,	hit,	hit }, //13
		{ hit,		stay,	stay,	stay,	stay,	stay,	hit,	hit,	hit,	hit }, //14
		{ hit,		stay,	stay,	stay,	stay,	stay,	hit,	hit,	hit,	hit }, //15
		{ hit,		stay,	stay,	stay,	stay,	stay,	hit,	hit,	hit,	hit }, //16
		{ stay,		stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay }, //17
		{ stay,		stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay }, //18
		{ stay,		stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay }, //19
		{ stay,		stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay }, //20
		{ stay,		stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay } //21
	};
	const Action soft_table[10][10] = {
		//A			2		3		4		5		6		7		8		9		10
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //A,A
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //2,A
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //3,A
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //4,A
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //5,A
		{ hit,		hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit,	hit }, //6,A
		{ hit,		stay,	stay,	stay,	stay,	stay,	stay,	stay,	hit,	hit }, //7,A
		{ stay,		stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay }, //8,A
		{ stay,		stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay }, //9,A
		{ stay,		stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay,	stay }, //10,A
	};
	const int CARDHEIGHT = 96, CARDWIDTH = 71;
	const int DEALERSIZE = 256;
	const int MAXDEALER = 11;
	const int MAXLEVEL = 7;
	const float SCREENRATIO = 0.2;
	const int DEMO_DELAY = 100;
	const int SCORE_DELAY = 15;
	int _dealer = 0;
	int _tick = 0, _nexttick = 0, _index = 0;
	int _fontsize = 48;
	int _cardwidth = (SCREEN_WIDTH * (1 - SCREENRATIO) - _fontsize) / 5;
	int _cardheight = _cardwidth * CARDHEIGHT / CARDWIDTH;
	int _level = 0, _currentxp = 0, _nextxp = 10;
	//bool hold[5] = {false, false, false, false, false};
	int bank = 500, displayedbank = bank;
	int defaultbet = 50, bet = defaultbet, bet2 = 0;
	//Button btnhold1, btnhold2, btnhold3, btnhold4, btnhold5;
	Button btndeal, btnhit, btnstay, btnsplit, btndoubledown, btninsuranceyes, btninsuranceno;
	Button btnincrease, btndecrease, btnconfirm;
	Button btnquit;
	Button btndealer;
	Outcome outcome = nothing, outcome2 = nothing;
	State _state = title;
	bool split = false, aces = false, hasinsurance = false;
	bool debug = false;
	bool demo = false;
	int demo_timeout = 100;
	std::vector<Message> _messages;
	Deck shoe;
	Hand hand;
	Hand hand2;
	Hand dealerhand;
	Outcome evaluate(Hand p, Hand d);
	std::string fill(std::string, int n, std::string pad);
	std::string translate_outcome(Outcome outcome);
	std::string translate_card(Card c);
	std::string translate_suit(int s);
	std::string translate_face(int f);
	Action get_strategy(Hand p, Hand d);
	std::string translate_action(Action a);
	void writeDialog(std::string txt);
public:
	Game();
	bool load();
	bool unload();
	void render(float interpolation);
	void update();
	void handleEvents(SDL_Event& e);
	void renderText(std::string message, int x, int y, LTexture* font, int subtype, int size);
	void drawBox(int color, int x, int y, int width, int height);
};

#endif