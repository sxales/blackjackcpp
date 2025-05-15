//TODO:
//DEALER DIALOG

var Blackjack = function() {
	var _width, _height;//in pixels
	var _tick = 0;
	var _nexttick = 0;
	var _state;
	var _fontsize = 40;
	var _cardwidth = 0, _cardheight = 0;
	const TITLE = 0, DEALERSELECT = 10, DEAL = 20, INSURANCE = 25, HITSTAY1 = 30, HITSTAY2 = 40, DEALERTURN = 50, SCORE = 60, RESET = 70, GAMEOVER = 666;//states
	const BLUE = 5, GRAY = 8, GREEN = 4, RED = 3, SUPER = 2, WHITE = 1, TEAL = 5, YELLOW = 6, PINK = 7, LOGO = 0; //font colors
	const DEALERSIZE = 512;
	const WIN = 1, LOSE = 0, PUSH = 2;
	const CLUBS = 0, DIAMONDS = 1, SPADES = 2, HEARTS = 3;//suits
	const CARDHEIGHT = 96, CARDWIDTH = 71;
	var messages = new Array();
	var mute = false;
	var volume = .2;
	var subscribers = new Array();
	const SCREENRATIO = .85; //percentage of screen that is table versus buttons
	var btndown;
	var split = false;
	var insurance = false;
	var debug = false;
	var aces = false;
	var pause = false;

	var dealerselection = 0;

	var joker = 0;

	var bank = 500;
	var displayedbank = bank;
	var defaultbet = 20;
	var bet = defaultbet;
	var bet2 = 0;

	var bjpayout = 2.5;

	var shoe = new Deck();

	var playerhand = new Hand();
	var playerhand2 = new Hand();
	var dealerhand = new Hand();

	var btnquit = new Button();
	var btnmute = new Button();

	//var btnshuffle = new Button();
	var btneasy = new Button();
	var btnnormal = new Button();
	var btnhard = new Button();

	var btndeal = new Button();

	var btnhit = new Button();
	var btnstay = new Button();
	var btnsplit = new Button();
	var btndoubledown = new Button();

	//var btnplayagain = new Button();

	var btnyes = new Button();
	var btnno = new Button();

	this.subscribe = function(s) {
		subscribers.push(s);
	};

	this.unsubscribe = function(s) {
		for (var i = 0; i<subscribers.length; i++) {
			if (subscribers[i] == s) {
				subscribers.splice(i);
				break;
			}
		}
	};

	notify = function(b) {
		for (var i = 0; i<subscribers.length; i++) {
			subscribers[i].call(this,b);
		}
	};

	this.init = function(w,h) {
		_height = h;
		_width = w;

		_cardheight = (_height*SCREENRATIO)/3.4;//Math.min((_height*SCREENRATIO)/3.5, (_width/4)*(CARDHEIGHT/CARDWIDTH));//(_height*SCREENRATIO-_fontsize*2.5)/4;
		_cardwidth= _cardheight*CARDWIDTH/CARDHEIGHT;

		_fontsize = (_width-Math.min((_width/2)-_fontsize*3/4, _cardheight))/13;

		//_fontsize = _width/20;

		_tick = 0;
		//_state = INITIALIZING;

		//btnshuffle.init("shuffle", _width - bw, _height - bh*(13/12), bw, bh);
		var buttonheight = _height*(1-SCREENRATIO)/2;
		var buttonwidth = _width/2;
		btndeal.init("deal", buttonwidth/2, _height*SCREENRATIO+buttonheight/2, buttonwidth, buttonheight);
		btnhit.init("hit", 0, _height*SCREENRATIO, buttonwidth, buttonheight);
		btnstay.init("stay", buttonwidth, _height*SCREENRATIO, buttonwidth, buttonheight);
		btnsplit.init("split", buttonwidth, _height*SCREENRATIO+buttonheight, buttonwidth, buttonheight);
		btndoubledown.init("double", 0, _height*SCREENRATIO+buttonheight, buttonwidth, buttonheight);
		//btnplayagain.init("again?", buttonwidth/2, _height*SCREENRATIO+buttonheight/2, buttonwidth, buttonheight);
		btnyes.init("yes", 0, _height*SCREENRATIO, buttonwidth, buttonheight);
		btnno.init("no", buttonwidth, _height*SCREENRATIO, buttonwidth, buttonheight);
		btnquit.init("X", _width-_fontsize*2, 0, _fontsize*2, _fontsize*2);
		btnmute.init(" ", 0, 0, _fontsize*2, _fontsize*2);

		var width = _width/3;
		var height = _height/5;
		var vpos = height/2;

		btnhard.init("", _fontsize/2, vpos, _width-_fontsize, height+width*0.25);
		vpos += height+width*0.25+_fontsize/2;
		btnnormal.init("", _fontsize/2, vpos, _width-_fontsize, height+width*0.25);
		vpos += height+width*0.25+_fontsize/2;
		btneasy.init("", _fontsize/2, vpos, _width-_fontsize, height+width*0.25);

		shoe.init(4);//4 decks in shoe
		shoe.shuffle();

		setVolume(volume);
		//if (typeof(Storage) !== "undefined") load();

		_state = TITLE;
	};

	this.keydown = function(evt) {
		if (evt.key == "m") {
			mute = !mute;
		}
		else if (evt.key == "d") {
			debug = !debug;
		}
	};

	this.mousedown = function(evt) {
		if (evt.button == 0) {
			btndown =  window.setTimeout(rightClick, 500, evt.clientX, evt.clientY);//long press
		}
	};

	this.mouseup = function(evt) {
		if (btndown) window.clearTimeout(btndown);
		if (evt.button == 0) {
			click(evt.clientX, evt.clientY); //left click
		}
		else rightClick(evt.clientX, evt.clientY);
	};

	this.mouseout = function(evt) {
		if (btndown) window.clearTimeout(btndown);
	};

	this.mousemove = function(evt) {
		btnquit.check(evt.clientX, evt.clientY);
		btnmute.check(evt.clientX, evt.clientY);
		if (_state == DEALERSELECT) {
			btneasy.check(evt.clientX, evt.clientY);
			btnnormal.check(evt.clientX, evt.clientY);
			btnhard.check(evt.clientX, evt.clientY);
		}
		else if (_state == DEAL) btndeal.check(evt.clientX, evt.clientY);
		else if (_state == HITSTAY1 || _state == HITSTAY2) {
			btnhit.check(evt.clientX, evt.clientY);
			btnstay.check(evt.clientX, evt.clientY);
			btnsplit.check(evt.clientX, evt.clientY);
			btndoubledown.check(evt.clientX, evt.clientY);
		}
		else if (_state == INSURANCE) {
			btnyes.check(evt.clientX, evt.clientY);
			btnno.check(evt.clientX, evt.clientY);
		}
		else if (_state == GAMEOVER) {
			//btnplayagain.check(evt.clientX, evt.clientY);
		}
	};

	this.touchstart = function(evt) {
		btndown =  window.setTimeout(rightClick, 500, evt.touches[0].pageX, evt.touches[0].pageY);//long press
	};

	this.touchend = function(evt) {
		if (btndown) window.clearTimeout(btndown);
		click(evt.changedTouches[0].pageX, evt.changedTouches[0].pageY); //left click
	};

	this.touchcancel = function(evt) {
		if (btndown) window.clearTimeout(btndown);
	};

	this.touchmove = function(evt) {
	};

	startsplit = function() {
		playerhand2.add(shoe.deal());
		if (playerhand2.value() == 21) {
			writeDialog("Player","Blackjack!",LOGO);
			if ((playerhand.value() == 21 && playerhand.size() == 2) || (playerhand.value() > 21)) {
				_nexttick = _tick + 5;
				_state = SCORE;
			}
			else _state = DEALERTURN;
		}
		else if (aces) _state = DEALERTURN
		else _state = HITSTAY2;
	};

	rightClick = function(inputX, inputY) {
	};

	placebet = function(b) {
		//var height = ((_height*SCREENRATIO*2/5)-(_fontsize*4));
		//var vpos = (_height*SCREENRATIO/5)+_fontsize*2;
		//var hpos = ((_width/2)-height)/2;
		//messages.push({ timeout: 25, tick: 0, x: hpos+(height-_fontsize*6)/2+_fontsize*3, y :vpos+_fontsize*2.25, delta: -1, type: RED, s: _fontsize, message: "-"+b});
		bank -= b;
	};

	winbet = function(b) {
		//var height = ((_height*SCREENRATIO*2/5)-(_fontsize*4));
		//var vpos = (_height*SCREENRATIO/5)+_fontsize*2;
		//var hpos = ((_width/2)-height)/2;
		//messages.push({ timeout: 50, tick: 0, x: hpos+(height-_fontsize*6)/2+_fontsize*3, y :vpos+_fontsize*2.25, delta: -1, type: GREEN, s: _fontsize, message: "+"+b});
		bank += b;
	};

	click = function(inputX, inputY) {
		if (btnquit.check(inputX, inputY)) {
			joker = 0;
			eraseSave();
			_state = TITLE;
			playerhand.reset();
			playerhand2.reset();
			dealerhand.reset();
		}
		//else if (btnmute.check(inputX, inputY)) mute = !mute;
		else if (_state == TITLE) {
			if (typeof(Storage) !== "undefined" && Number(localStorage.getItem("bank")) > 0) {
				load();
				displayedbank = bank;
				bet = defaultbet;
				_state = DEAL;
			}
			else {
				_state = DEALERSELECT;
			}
		}
		else if (_state == DEALERSELECT) {
			if (btneasy.check(inputX, inputY)) {
				dealerselection = 0;
				bank = 500;
				displayedbank = bank;
				defaultbet = 50;
				bjpayout = 2.5;
				bet = defaultbet;
				_state = DEAL;
			}
			else if (btnnormal.check(inputX, inputY)) {
				dealerselection = 1;
				bank = 500;
				displayedbank = bank;
				defaultbet = 100;
				bjpayout = 2.2;
				bet = defaultbet;
				_state = DEAL;
			}
			else if (btnhard.check(inputX, inputY)) {
				dealerselection = 2;
				bank = 500;
				displayedbank = bank;
				defaultbet  = 250;
				bjpayout = 2;
				bet = defaultbet;
				_state = DEAL;
			}
		}
		else if (_state == DEAL && btndeal.check(inputX, inputY)) {
			bet = defaultbet;

			//deal
			playerhand.reset();
			playerhand2.reset();
			dealerhand.reset();

			playerhand.add(shoe.deal());
			dealerhand.add(shoe.deal());
			playerhand.add(shoe.deal());
			dealerhand.add(shoe.deal());

			placebet(bet);

			if (dealerhand.get(1).face == 1) {
				//insurance
				_state = INSURANCE;
			}
			else if (playerhand.value() == 21 || dealerhand.value() == 21) {
				if (playerhand.value() == 21) writeDialog("Player","Blackjack!",LOGO);
				if (dealerhand.value() == 21) writeDialog("Dealer","Blackjack!",LOGO);
				//bank += bet*2.5;
				_nexttick = _tick + 5;
				_state = SCORE;
			}
			else _state = HITSTAY1;
		}
		else if (_state == HITSTAY1) {
			if (btnstay.check(inputX, inputY)) {
				if (split) startsplit();
				else _state = DEALERTURN;
			}
			else if (btnhit.check(inputX, inputY)) {
				playerhand.add(shoe.deal());
				if (playerhand.value() > 21) {
					writeDialog("Player","Bust!",LOGO);
					if (split) startsplit();
					else {
						_nexttick = _tick + 5;
						_state = SCORE; //bust
					}
				}
			}
			else if (playerhand.value() >= 9 && playerhand.value() <= 11 && btndoubledown.check(inputX, inputY) && bank >= bet) {
				playerhand.add(shoe.deal());
				placebet(bet);
				bet *= 2;
				if (split) startsplit();
				else _state = DEALERTURN;
			}
			else if (!split && playerhand.size() == 2 && playerhand.get(0).face == playerhand.get(1).face && btnsplit.check(inputX, inputY) && bank >= bet) {
				split = true;
				bet2 = bet;
				placebet(bet2);

				playerhand2.reset();
				playerhand2.add(playerhand.get(1));
				var temp = playerhand.get(0);
				playerhand.reset();
				playerhand.add(temp);
				playerhand.add(shoe.deal());

				if (playerhand.get(0).face == 1) aces= true;

				if (playerhand.value() == 21) {
					writeDialog("Player","Blackjack!",LOGO);
					//bank += bet2*2.5;
					startsplit();
				}
				else if (aces) startsplit();
			}
		}
		else if (_state == HITSTAY2) {
			if (btnstay.check(inputX, inputY)) {
				_state = DEALERTURN;
			}
			else if (btnhit.check(inputX, inputY)) {
				playerhand2.add(shoe.deal());
				if (playerhand2.value() > 21) {
					writeDialog("Player","Bust!",LOGO);
					if (playerhand.value() > 21) {
						_nexttick = _tick + 5;
						_state = SCORE; //bust
					}
					else _state = DEALERTURN;
				}
			}
			else if (playerhand2.value() >= 9 && playerhand2.value() <= 11 && btndoubledown.check(inputX, inputY) && bank >= bet) {
				playerhand2.add(shoe.deal());
				placebet(bet2);
				bet2 *= 2;
				_state = DEALERTURN;
			}
		}
		else if (_state == INSURANCE) {
			if (btnyes.check(inputX, inputY)) {
				insurance = true;
				bet = Math.round(bet*0.5);
				if (dealerhand.value() == 21 && dealerhand.size() == 2) {
					if (playerhand.value() == 21 && playerhand.size() == 2) {
						_state = SCORE;
					}
					else {
						bank += bet*2;
						writeDialog("Dealer","Blackjack!",LOGO);
						_nexttick = _tick + 5;
						_state = SCORE;
					}
				}
				else {
					if (playerhand.value() == 21 && playerhand.size() == 2) {
						writeDialog("Player","Blackjack!",LOGO);
						_nexttick = _tick + 5;
						//bank += bet2*2.5;
						_state = SCORE;
					}
					else _state = HITSTAY1;
				}
			}
			else if (btnno.check(inputX, inputY)) {
				insurance = false;
				if (dealerhand.value() == 21 && dealerhand.size() == 2) {
					if (playerhand.value() == 21 && playerhand.size() == 2) {
						_state = SCORE;
					}
					else {
						writeDialog("Dealer","Blackjack!",LOGO);
						_nexttick = _tick + 5;
						_state = SCORE;
					}
				}
				else {
					if (playerhand.value() == 21 && playerhand.size() == 2) {
						writeDialog("Player","Blackjack!",LOGO);
						_nexttick = _tick + 5;
						//bank += bet2*2.5;
						_state = SCORE;
					}
					else _state = HITSTAY1;
				}
			}
		}
	};

	this.draw = function(ctx) {
		ctx.clearRect(0, 0, _width, _height);

		//draw title screen
		if (_state == TITLE) {
			ch = _height/6;
			cw = _width/5;
			var row = Math.ceil(_height/ch);
			var col = Math.ceil(_width/cw);

			for (let i = 0; i < row*col; i++) {
				/*if (joker < row*col) {
					if (i < joker) ctx.drawImage(resourceRepository.cards, CARDWIDTH*(13), CARDHEIGHT*(2+((i)%2)), CARDWIDTH, CARDHEIGHT, cw*(i%col), ch*(Math.floor(i/col)), cw, ch);
					else ctx.drawImage(resourceRepository.cards, CARDWIDTH*(13), CARDHEIGHT*(i%2), CARDWIDTH, CARDHEIGHT, cw*(i%col), ch*(Math.floor(i/col)), cw, ch);
				}
				else {
					if (i < joker-(row*col)) ctx.drawImage(resourceRepository.cards, CARDWIDTH*(13), CARDHEIGHT*(i%2), CARDWIDTH, CARDHEIGHT, cw*(i%col), ch*(Math.floor(i/col)), cw, ch);
					else ctx.drawImage(resourceRepository.cards, CARDWIDTH*(13), CARDHEIGHT*(2+((i)%2)), CARDWIDTH, CARDHEIGHT, cw*(i%col), ch*(Math.floor(i/col)), cw, ch);
				}*/
				if (_tick%2 == 0) ctx.drawImage(resourceRepository.cards, CARDWIDTH*(13), CARDHEIGHT*(i%2), CARDWIDTH, CARDHEIGHT, cw*(i%col), ch*(Math.floor(i/col)), cw, ch);
				else ctx.drawImage(resourceRepository.cards, CARDWIDTH*(13), CARDHEIGHT*((i+1)%2), CARDWIDTH, CARDHEIGHT, cw*(i%col), ch*(Math.floor(i/col)), cw, ch);

			}
			if (joker > row*col*2) joker = 0;

			/*var height = _fontsize*2;
			var width = _width - _fontsize*4;
			var vpos = (_height-height)/2;
			var hpos = _fontsize*2;
			//draw bank
			drawBox(ctx, 20, hpos, vpos, width, height);
			writeMessage(ctx, "Blackjack!", SUPER, (_width-_fontsize*10)/2, vpos+_fontsize*0.5, _fontsize);

			if (joker%2 == 0) writeMessage(ctx, "Touch to start", BLUE, (_width-_fontsize*14)/2, _height*0.75, _fontsize);*/
			var w = _width/2-_fontsize*2;
			var vpos = (_height-w)/2;
			var hpos = (_width-w)/2;
			//draw dealer
			drawBox(ctx, 20, hpos, vpos, w, w);
			ctx.drawImage(resourceRepository.dealer, DEALERSIZE*0, DEALERSIZE*(Math.floor(joker/10)%3), DEALERSIZE, DEALERSIZE, hpos+_fontsize*0.25, vpos+_fontsize*0.25, w-_fontsize*0.5, w-_fontsize*0.5);

			var height = _fontsize*2;
			var width = _fontsize*13;
			var vpos = (vpos - _fontsize*2)/2;
			var hpos = (_width-width)/2;

			drawBox(ctx, 20, hpos, vpos-_fontsize/2, width, height);
			writeMessage(ctx, "Blackjack!", SUPER, (_width-_fontsize*10)/2, vpos, _fontsize);

			if (joker%2 == 0) writeMessage(ctx, "Touch to start", BLUE, (_width-_fontsize*14)/2, w+(_height-w)/2+vpos, _fontsize);
		}
		else if (_state == DEALERSELECT) {
			writeMessage(ctx, "Select dealer", SUPER, (_width-_fontsize*13)/2, _fontsize, _fontsize);
			//var height = ((_height*SCREENRATIO*2/5)-(_fontsize*4));
			var width = _width/3;
			var height = _height/5;
			var vpos = height/2;
			var hpos = _fontsize/2;
			var fs = Math.min(height/5.25, (_width-_fontsize*2-width)/10);
			var offset = (height-fs*5.25)/2;
			//hard
			drawBox(ctx, 20, _fontsize, vpos+width*0.25, _width-_fontsize*2, height);
			drawBox(ctx, 20, hpos, vpos, width, width);
			ctx.drawImage(resourceRepository.dealer, 0, DEALERSIZE*2, DEALERSIZE, DEALERSIZE, hpos+5, vpos+5, width-12, width-12);
			writeMessage(ctx, "Blackjack", WHITE, hpos+width+fs/2, vpos+offset+width*0.25+fs/2, fs);
			writeMessage(ctx, "pays 1:1", WHITE, hpos+width+fs/2, vpos+offset+width*0.25+fs*1.75, fs);
			writeMessage(ctx, "Bet 250", WHITE, hpos+width+fs/2, vpos+offset+width*0.25+fs*3.5, fs);
			//normal
			vpos += height+width*0.25+_fontsize/2;
			drawBox(ctx, 20, _fontsize, vpos+width*0.25, _width-_fontsize*2, height);
			drawBox(ctx, 20, hpos, vpos, width, width);
			ctx.drawImage(resourceRepository.dealer, 0, DEALERSIZE*1, DEALERSIZE, DEALERSIZE, hpos+5, vpos+5, width-12, width-12);
			writeMessage(ctx, "Blackjack", WHITE, hpos+width+fs/2, vpos+offset+width*0.25+fs/2, fs);
			writeMessage(ctx, "pays 6:5", WHITE, hpos+width+fs/2, vpos+offset+width*0.25+fs*1.75, fs);
			writeMessage(ctx, "Bet 100", WHITE, hpos+width+fs/2, vpos+offset+width*0.25+fs*3.5, fs);
			//easy
			vpos += height+width*0.25+_fontsize/2;
			drawBox(ctx, 20, _fontsize, vpos+width*0.25, _width-_fontsize*2, height);
			drawBox(ctx, 20, hpos, vpos, width, width);
			ctx.drawImage(resourceRepository.dealer, 0, DEALERSIZE*0, DEALERSIZE, DEALERSIZE, hpos+5, vpos+5, width-12, width-12);
			writeMessage(ctx, "Blackjack", WHITE, hpos+width+fs/2, vpos+offset+width*0.25+fs/2, fs);
			writeMessage(ctx, "pays 3:2", WHITE, hpos+width+fs/2, vpos+offset+width*0.25+fs*1.75, fs);
			writeMessage(ctx, "Bet 50", WHITE, hpos+width+fs/2, vpos+offset+width*0.25+fs*3.5, fs);

			//btneasy.draw(ctx);
			//btnnormal.draw(ctx);
			//btnhard.draw(ctx);
		}

		//draw turn marker
		var splitscale = 1;
		if (split) var splitscale = ((_width/2-_fontsize)/1.8)/_cardwidth;//0.65;
		if (_state == HITSTAY1) {
			writeMessage(ctx, "<", YELLOW, _fontsize+_cardwidth*splitscale+(_cardwidth*splitscale*0.2*(playerhand.size()-1)), _cardheight*2.3+(_cardheight-_fontsize)/2, _fontsize);
		}
		else if (_state == HITSTAY2) {
			writeMessage(ctx, "<", YELLOW, _width/2+_fontsize+_cardwidth*splitscale+(_cardwidth*splitscale*0.2*(playerhand2.size()-1)), _cardheight*2.3+(_cardheight-_fontsize)/2, _fontsize);
		}
		else if (_state == DEALERTURN) {
			writeMessage(ctx, "<", YELLOW, _fontsize+_cardwidth+(_cardwidth*0.2*(dealerhand.size()-1)), _cardheight*0.1+(_cardheight-_fontsize)/2, _fontsize);
		}

		//draw cards
		var rollover = Math.floor((_width/2)/(_cardwidth*0.2) - 4);
		if (split) {
			var splitscale = ((_width/2-_fontsize)/1.8)/_cardwidth;//0.65;
			rollover = Math.floor((_width/2)/(_cardwidth*splitscale*0.2) - 4);
			for (let i = 0; i < playerhand.size(); i++) {
				ctx.drawImage(resourceRepository.cards, CARDWIDTH*(playerhand.get(i).face-1), CARDHEIGHT*playerhand.get(i).suit, CARDWIDTH, CARDHEIGHT, _fontsize/2+(_cardwidth*splitscale*.20)*(i%rollover), (Math.floor(i/rollover)*(_cardheight*0.20))+_cardheight*2.3+_cardheight*((1-splitscale)/2), _cardwidth*splitscale, _cardheight*splitscale);
			}
			for (let i = 0; i < playerhand2.size(); i++) {
				ctx.drawImage(resourceRepository.cards, CARDWIDTH*(playerhand2.get(i).face-1), CARDHEIGHT*playerhand2.get(i).suit, CARDWIDTH, CARDHEIGHT, _fontsize/2+_width/2+(_cardwidth*splitscale*.20)*(i%rollover), (Math.floor(i/rollover)*(_cardheight*0.20))+_cardheight*2.3+_cardheight*((1-splitscale)/2), _cardwidth*splitscale, _cardheight*splitscale);
			}
		}
		else {
			for (let i = 0; i < playerhand.size(); i++) {
				ctx.drawImage(resourceRepository.cards, CARDWIDTH*(playerhand.get(i).face-1), CARDHEIGHT*playerhand.get(i).suit, CARDWIDTH, CARDHEIGHT, _fontsize/2+(_cardwidth*.20)*i, _cardheight*2.3, _cardwidth, _cardheight);
			}
		}

		if (_state > DEAL && _state < DEALERTURN) {
			ctx.drawImage(resourceRepository.cards, CARDWIDTH*13, 0, CARDWIDTH, CARDHEIGHT, _fontsize/2, _cardheight*0.1, _cardwidth, _cardheight);
			ctx.drawImage(resourceRepository.cards, CARDWIDTH*(dealerhand.get(1).face-1), CARDHEIGHT*dealerhand.get(1).suit, CARDWIDTH, CARDHEIGHT, _fontsize/2+(_cardwidth*.20)*1, _cardheight*0.1, _cardwidth, _cardheight);
		}
		else if (_state >= DEALERTURN && _state < GAMEOVER) {
			for (let i = 0; i < dealerhand.size(); i++) {
				ctx.drawImage(resourceRepository.cards, CARDWIDTH*(dealerhand.get(i).face-1), CARDHEIGHT*dealerhand.get(i).suit, CARDWIDTH, CARDHEIGHT, _fontsize/2+(_cardwidth*.20)*i, _cardheight*0.1, _cardwidth, _cardheight);
			}
		}
		//draw scoreboard
		if (_state >= DEAL && _state < GAMEOVER) {
			if (displayedbank < bank) displayedbank += Math.round(bet*0.1);
			else displayedbank = bank;

			//draw bank
			var height = Math.min((_width/2)-_fontsize*3/4, _cardheight);
			var vpos = _cardheight*1.2+(_cardheight-height)/2;
			var fs = Math.min((_width/2)/8, height/6);
			var w = _width/2-_fontsize*3/4;
			drawBox(ctx, 20, _fontsize/2, vpos, w, height);
			vpos += (height-fs*5.75)/2;
			writeMessage(ctx, "Bank:", WHITE, (_width/2-fs*5)/2, vpos+fs*0.50, fs);
			writeMessage(ctx, zeroFill(displayedbank, 6), PINK, (_width/2-fs*6)/2, vpos+fs*1.75, fs);
			writeMessage(ctx, "Bet:", WHITE, (_width/2-fs*4)/2, vpos+fs*3, fs);
			var txt = ""+(bet+bet2);
			writeMessage(ctx, txt, PINK, (_width/2-fs*txt.length)/2, vpos+fs*4.25, fs);

			//draw dealer
			var width = Math.min((_width/2)-_fontsize*3/4, _cardheight);
			hpos = _width/2+(_width/2-width)/2;
			vpos = _cardheight*1.2+(_cardheight-width)/2;
			//vpos = _fontsize/2;
			drawBox(ctx, 20, hpos, vpos, width, width);
			ctx.drawImage(resourceRepository.dealer, 0, DEALERSIZE*dealerselection, DEALERSIZE, DEALERSIZE, hpos+5, vpos+5, width-12, width-12);
		}

		//draw buttons
		if (_state == DEAL) btndeal.draw(ctx);
		else if (_state == HITSTAY1 || _state == HITSTAY2) {
			btnhit.draw(ctx);
			btnstay.draw(ctx);
			if (_state == HITSTAY1 && playerhand.size() == 2 && playerhand.value() >= 9 && playerhand.value() <= 11 && bank >= bet) btndoubledown.draw(ctx);
			else if (_state == HITSTAY2 && playerhand2.size() == 2 && playerhand2.value() >= 9 && playerhand2.value() <= 11 && bank >= bet) btndoubledown.draw(ctx);
			if (!split && playerhand.size() == 2 && playerhand.get(0).face == playerhand.get(1).face && bank >= bet) btnsplit.draw(ctx);
		}
		else if (_state == INSURANCE) {
			//var width = _;
			var width = Math.min((_width/2)-_fontsize*3/4, _cardheight);
			var hpos = _width/2+(_width/2-width)/2-_fontsize*11;
			//var hpos = _width/2+(_width/2-_fontsize*11)/2;
			var vpos = _cardheight*1.2+(_cardheight-width)/2;
			//var vpos = (_height*SCREENRATIO/5)+_fontsize/2;
			drawBox(ctx, 20, hpos, vpos, _fontsize*11, _fontsize*2);
			writeMessage(ctx, "Insurance?", LOGO, hpos+_fontsize/2, vpos+_fontsize/2, _fontsize);
			btnyes.draw(ctx);
			btnno.draw(ctx);
		}

		//draw outcomes
		if (_state >= SCORE && _state < GAMEOVER) {
			//playerhand 1
			var fs = (_width/2)/10;
			var vpos = _height*SCREENRATIO;//_cardheight*3+_fontsize*2+(_cardheight-fs)/2;
			var hpos = _fontsize/2+((_cardwidth+(playerhand.size()-1)*(_cardwidth*0.2))-(fs*5))/2;
			if (split) hpos = (_width/2-fs*5)/2;

			switch (evaluate(playerhand, dealerhand)) {
				case LOSE:
					writeMessage(ctx, "Lose!", RED, hpos, vpos, fs);
					break;
				case PUSH:
					writeMessage(ctx, "Push!", GRAY, hpos, vpos, fs);
					break;
				case WIN:
					writeMessage(ctx, "Wins!", GREEN, hpos, vpos, fs);
					break;
			}

			//playerhand 2
			if (split) {
				vpos =  _height*SCREENRATIO;//_cardheight*2+_fontsize*1.5+(_cardheight-fs)/2;
				hpos = _width/2+(_width/2-fs*5)/2;//_fontsize/2+_width/2+((_cardwidth+(playerhand2.size()-1)*(_cardwidth*0.2))-(fs*5))/2;
				switch (evaluate(playerhand2, dealerhand)) {
					case LOSE:
						writeMessage(ctx, "Lose!", RED, hpos, vpos, fs);
						break;
					case PUSH:
						writeMessage(ctx, "Push!", GRAY, hpos, vpos, fs);
						break;
					case WIN:
						writeMessage(ctx, "Wins!", GREEN, hpos, vpos, fs);
						break;
				}
			}
		}

		//draw messages
		for(let i=0; i<messages.length; i++) {
			if (i >= 2) break;
			messages[i].y += messages[i].delta;
			if (i%2 == 1) {
				if (messages[i].message.length > messages[i-1].message.length) { //blackjack
					drawBox(ctx, 20, messages[i].x-messages[i].s/2, messages[i-1].y-messages[i-1].s/2, messages[i].message.length*messages[i].s+messages[i].s, messages[i].s*3.25);
				}
				else { //bust
					drawBox(ctx, 20, messages[i-1].x-messages[i-1].s/2, messages[i-1].y-messages[i-1].s/2, messages[i-1].message.length*messages[i-1].s+messages[i-1].s, messages[i-1].s*3.25);
				}
				writeMessage(ctx, messages[i].message, messages[i].type, messages[i].x, messages[i].y, messages[i].s);
				writeMessage(ctx, messages[i-1].message, messages[i-1].type, messages[i-1].x, messages[i-1].y, messages[i-1].s);
			}

			//if (messages[i].tick++ >= messages[i].timeout) messages.splice(i--, 1); //remove
		}

		if (_state == GAMEOVER) {
			//var height = ((_height*SCREENRATIO*2/5)-(_fontsize*4));
			var width = _width/2;

			//draw dealer
			var vpos = (_height/2-width)/2;
			var hpos = (_width-width)/2;
			drawBox(ctx, 20, hpos, vpos, width, width);
			ctx.drawImage(resourceRepository.dealer, DEALERSIZE, DEALERSIZE*dealerselection, DEALERSIZE, DEALERSIZE, hpos+5, vpos+5, width-12, width-12);

			hpos = ((_width)-(_fontsize*10))/2;
			writeMessage(ctx, "Game over!", SUPER, hpos, vpos-_fontsize*2, _fontsize);

			hpos = ((_width)-(_fontsize*8))/2;
			vpos += width+_fontsize;
			drawBox(ctx, 20, hpos-_fontsize/2, vpos-_fontsize/2, _fontsize*9, _fontsize*2);
			writeMessage(ctx, "Too bad!", LOGO, hpos, vpos, _fontsize);

			var timer = ""+Math.round((_nexttick - _tick)/3+1);
			writeMessage(ctx, timer, YELLOW, (_width-_fontsize*timer.length)/2, _height-(vpos/2), _fontsize);
		}
		else if (_state > TITLE) {
			btnquit.draw(ctx);
			//btnmute.draw(ctx);
			//if (!mute) ctx.drawImage(resourceRepository.volume, 0, 0, 64, 64, _fontsize/2, _fontsize/2, _fontsize, _fontsize);
			//else ctx.drawImage(resourceRepository.volume, 64, 0, 64, 64, _fontsize/2, _fontsize/2, _fontsize, _fontsize);
		}
	};

	writeDialog = function(t1, t2, c) {
		var vpos = _cardheight*1.2+_fontsize/2+(_cardheight-Math.min((_width/2)-_fontsize*3/4, _cardheight))/2;;
		var hpos = _width/2+(_width/2-Math.min((_width/2)-_fontsize*3/4, _cardheight))/2
		if (t2.length > t1.length) { //blackjack
				messages.push({ timeout: 5, tick: 0, x: hpos-(t1.length+2.5)*_fontsize, y :vpos, delta: 0, type: c, s: _fontsize, message: t1});
				messages.push({ timeout: 5, tick: 0, x: hpos-(t2.length+0.5)*_fontsize, y :vpos+_fontsize*1.25, delta: 0, type: c, s: _fontsize, message: t2});
		}
		else { //bust
			messages.push({ timeout: 5, tick: 0, x: hpos-(t1.length+0.5)*_fontsize, y :vpos, delta: 0, type: c, s: _fontsize, message: t1});
			messages.push({ timeout: 5, tick: 0, x: hpos-(t2.length+1)*_fontsize, y :vpos+_fontsize*1.25, delta: 0, type: c, s: _fontsize, message: t2});
		}

	};

	evaluate = function(p, d) {
		if (p.value() > 21) {
			//player bust
			return LOSE;
		}
		else if (d.value() > 21) {
			//dealer bust
			return WIN;
		}
		else if (p.value() > d.value()) {
			return WIN;
		}
		else if (p.value() == d.value()) {
			if (p.value() == 21 && p.size() == 2 && d.size() > 2) return WIN; //player blackjack beats dealer 21
			else if (d.value() == 21 && d.size() == 2 && p.size() > 2) return LOSE; //dealer blackjack beats player 21
			else return PUSH;
		}
		else {
			return LOSE;
		}
	};

	this.update = function() {
		++_tick;
		if (_state == TITLE) {
			joker++;
			/*
			var ch = _height*SCREENRATIO/5;
			var cw = ch*CARDWIDTH/CARDHEIGHT;
			var row = Math.ceil(_height/ch);
			var col = Math.ceil(_width/cw);
			jokerx = Math.round(Math.random()*col);
			jokery = Math.round(Math.random()*row);*/
			//_nexttick = _tick + 5;//delay between cards
		}
		else if (_state < DEALERTURN) _nexttick = _tick + 2;
		else if (_state == DEALERTURN && _nexttick < _tick) {
			_nexttick = _tick + 5;//delay between cards
			if (dealerhand.value() == 21 && dealerhand.size() == 2) {
				writeDialog("Dealer","Blackjack!",LOGO);
				_state = SCORE;
			}
			else if (((!split && playerhand.value() < 22) || (split && (playerhand.value() < 22 || playerhand2.value() < 22))) && dealerhand.value() < 17) {
				dealerhand.add(shoe.deal());
				if (dealerhand.value() > 21) {
					writeDialog("Dealer","Bust!",LOGO);
					_state = SCORE;
				}
			}
			else _state = SCORE;
		}
		else if (_state == SCORE) {
			switch (evaluate(playerhand, dealerhand)) {
				case LOSE:
					if (insurance && dealerhand.value() == 21 && dealerhand.size() == 2) winbet(bet*2);
					break;
				case PUSH:
					winbet(bet);
					break;
				case WIN:
					if (playerhand.value() == 21 && playerhand.size() == 2) winbet(Math.round(bet*bjpayout));
					else winbet(bet*2);
					break;
			}

			if (split) {
				switch (evaluate(playerhand2, dealerhand)) {
					case LOSE:
						break;
					case PUSH:
						winbet(bet2);
						break;
					case WIN:
						if (playerhand2.value() == 21 && playerhand2.size() == 2) winbet(Math.round(bet2*bjpayout));
						else winbet(bet2*2);
						break;
				}
			}
			bet = defaultbet;
			bet2 = 0;
			save();
			_nexttick = _tick + 5;
			_state = RESET;
		}
		else if (_state == RESET && _nexttick < _tick) {
			split = false;
			insurance = false;
			aces = false;
			playerhand.reset();
			playerhand2.reset();
			dealerhand.reset();

			if (bank < bet) {
				_nexttick = _tick + 25;
				if (!mute) resourceRepository.gameover.play();
				_state = GAMEOVER;
			}
			else _state = DEAL;
		}
		else if (_state == GAMEOVER && _nexttick < _tick) {
			//bank = 500;
			//displayedbank = bank;
			//shoe.shuffle();
			eraseSave();
			_state = TITLE;
		}

		//update messages
		for(let i=0; i<messages.length; i++) {
			if (i >= 2) break;
			if (messages[i].tick++ >= messages[i].timeout) messages.splice(i--, 1); //remove
		}
	};

	save = function() {
		if (typeof(Storage) !== "undefined") {
			// Code for localStorage/sessionStorage.
			localStorage.setItem("bank", bank);
			localStorage.setItem("bet", defaultbet);
			localStorage.setItem("bjpayout", bjpayout);
			localStorage.setItem("dealerselection", dealerselection);
		}
		else {
		  // Sorry! No Web Storage support..
		}
	};

	load = function() {
		bank = Number(localStorage.getItem("bank"));
		defaultbet = Number(localStorage.getItem("bet"));
		bjpayout = Number(localStorage.getItem("bjpayout"));
		dealerselection =  Number(localStorage.getItem("dealerselection"));
	};

	eraseSave = function() {
		localStorage.removeItem("bank");
		localStorage.removeItem("bet");
		localStorage.removeItem("bjpayout");
		localStorage.removeItem("dealerselection");
	};

	setVolume = function(v) {
		volume = v;

		resourceRepository.gameover.volume = volume;
		resourceRepository.win.volume = volume;
	};

}