#include <Gamebuino.h>
#include <SPI.h>
Gamebuino gb;

void settings(bool & first, int & player_h, int & player_vy, int & player_edge, int & op_h, int & op_vy, int & op_edge, int & ball_vx, int & ball_vy);

//##### VARIABLES

//player settings
int player_score = 0;
int player_h = 14; //set up in settings()
int player_w = 3;
int player_x = 0;
int player_y = (LCDHEIGHT - player_h) / 2;
int player_vy = 2; //set up in settings()
int player_edge; //paddle edge size; set up in settings()

//opponent settings
int op_score = 0;
int op_h = 14; //set up in settings()
int op_w = 3;
int op_x = LCDWIDTH - op_w;
int op_y = (LCDHEIGHT - op_h) / 2;
int op_vy = 2; //set up in settings()
int op_edge; //paddle edge size; set up in settings()

//ball settings
int ball_size = 5;
int ball_vx = 2; //set up in settings()
int ball_vy = 2; //set up in settings()
int ball_x = LCDWIDTH - ball_size - op_w - 1;
int ball_y = (LCDHEIGHT - ball_size) / 2;

//other settings
bool end = false;
char player[4];
int score_to_end = 7;
bool first = true; //first game?

void setup() {
	gb.begin();
	gb.titleScreen(F("Pong"));
	gb.pickRandomSeed();
	gb.battery.show = true;
	gb.display.fontSize = 1;
}

//##### MAIN

void loop() {
    if(gb.update()){

		//settings / level
		if(first){
			settings(first, player_h, player_vy, player_edge, op_h, op_vy, op_edge, ball_vx, ball_vy);
		}

		if(!end && !first){
			//draw score
			gb.display.cursorX = (LCDWIDTH / 2) - 15;
			gb.display.print(player_score);
			//draw op score
			gb.display.cursorX = (LCDWIDTH / 2) + 10;
			gb.display.print(op_score);

			//draw player paddle
			gb.display.fillRect(player_x, player_y, player_w, player_h);

			//bounce ball from player paddle
			if(ball_x == player_w && (ball_y + ball_size >= player_y && ball_y + (ball_size / 2) <= player_y + player_edge)){
				ball_vx = -ball_vx;
				ball_vy > 0 ? ball_vy += 1 : ball_vy -= 1;
				gb.sound.playNote(100, 1, 0);
			}else if(ball_x == player_w && (ball_y + (ball_size / 2) > player_y + player_edge && ball_y + (ball_size / 2) < player_y + player_h - player_edge)){
				ball_vx = -ball_vx;
				ball_vy > 0 ? ball_vy -= 1 : ball_vy += 1;
				gb.sound.playNote(1, 1, 0);
			} else if(ball_x == player_w && (ball_y + (ball_size / 2) >= player_y + player_h - player_edge && ball_y <= player_y + player_h)){
				ball_vx = -ball_vx;
				ball_vy > 0 ? ball_vy += 1 : ball_vy -= 1;
				gb.sound.playNote(100, 1, 0);
			}

			//bounce ball from oponent paddle
			if(ball_x == LCDWIDTH - ball_size && (ball_y + ball_size >= op_y && ball_y + (ball_size / 2) <= op_y + op_edge)){
				ball_vx = -ball_vx;
				ball_vy > 0 ? ball_vy += 1 : ball_vy -= 1;
				gb.sound.playNote(100, 1, 0);
			} else if(ball_x == LCDWIDTH - ball_size && (ball_y + (ball_size / 2) > op_y + op_edge && ball_y + (ball_size / 2) < op_y + op_h - op_edge)){
				ball_vx = -ball_vx;
				ball_vy > 0 ? ball_vy -= 1 : ball_vy += 1;
				gb.sound.playNote(1, 1, 0);
			} else if(ball_x == LCDWIDTH - ball_size && (ball_y + (ball_size / 2) >= op_y + op_h - op_edge && ball_y <= op_y + op_h)){
				ball_vx = -ball_vx;
				ball_vy > 0 ? ball_vy += 1 : ball_vy -= 1;
				gb.sound.playNote(100, 1, 0);
			}

			//move player paddle
			if(gb.buttons.repeat(BTN_UP, 1)){
				player_y = max(0, player_y - player_vy);
			} else if(gb.buttons.repeat(BTN_DOWN, 1)){
				player_y = min(LCDHEIGHT - player_h, player_y + player_vy);
			}

			//opponent move
			ball_y - (ball_size / 2) > op_y + (op_h / 2)	 ?
				op_y = min(LCDHEIGHT-op_h, op_y+op_vy) :
				op_y = max(0, op_y-op_vy);

			//move ball
			ball_x += ball_vx;
			ball_y += ball_vy;

			//bounce ball from walls
			if((ball_y + ball_size) >= LCDHEIGHT){
				ball_vy = -ball_vy;
				//gb.sound.playTick();
			} else if(ball_y <= 0){
				ball_y = 0;
				ball_vy = -ball_vy;
				//gb.sound.playTick();
			}

			//score for player
			if(ball_x + ball_size >= LCDWIDTH + 4){
				player_score += 1;
				ball_x = player_w + 2;
				ball_y = (LCDHEIGHT - ball_size) / 2;
				ball_vy = 2;
				ball_vx = abs(ball_vx);
				delay(1000);
			}

			//score for op
			if(ball_x <= -5){
				op_score += 1;
				ball_x = LCDWIDTH - ball_size - op_w - 1;
				ball_y = (LCDHEIGHT - ball_size) / 2;
				ball_vy = 2;
				ball_vx = - abs(ball_vx);
				delay(1000);
			}

			//draw ball
			gb.display.fillRect(ball_x, ball_y, ball_size, ball_size);
			//draw opponent
			gb.display.fillRect(op_x, op_y, op_w, op_h);
			//draw h line
			gb.display.drawFastVLine((LCDWIDTH / 2), 0, LCDHEIGHT);

			//check if game ends if so reset status
			if(player_score >= score_to_end || op_score >= score_to_end){
				player_score == score_to_end ? strncpy(player, "You\0", 4) : strncpy(player, "CPU\0", 4);
				gb.display.clear();
				gb.display.print(player);
				gb.display.println(" won");
				gb.display.println(F("Do you want to"));
				gb.display.println(F("play again?"));
				gb.display.println(F("  A - yes"));
				gb.display.print(F("  B - no"));
				if(gb.buttons.repeat(BTN_A, 1)){
					//player settings
					player_score = 0;
					player_x = 0;
					player_y = (LCDHEIGHT - player_h) / 2;

					//opponent settings
					op_score = 0;
					op_x = LCDWIDTH - op_w;
					op_y = (LCDHEIGHT - op_h) / 2;

					//ball
					ball_vx = -2;
					ball_vy = 2;
					ball_x = LCDWIDTH - ball_size - op_w - 1;
					ball_y = (LCDHEIGHT - ball_size) / 2;

					//other settings
					end = false;
					first = true;
				}
			}

		if(gb.buttons.repeat(BTN_B, 1)){
				gb.titleScreen(F("Pong"));
			}
		}
	}
}

//##### FUNCTIONS

void settings(bool & first, int & player_h, int & player_vy, int & player_edge, int & op_h, int & op_vy, int & op_edge, int & ball_vx, int & ball_vy){
	gb.display.println(F("Choose level:"));
	gb.display.println(F("  UP - easy"));
	gb.display.println(F("  RIGHT - normal"));
	gb.display.print(F("  DOWN - hard"));

	//easy
	if(gb.buttons.repeat(BTN_UP, 1)){
		first = false;
		//player setings
		player_h = 16;
		player_vy = 3;
		player_edge = 4;

		//opponent settings
		op_h = 14;
		op_vy = 1;
		op_edge = 3;

		//ball
		ball_vx = 2;
		ball_vy = 2;
	}

	//normal
	if(gb.buttons.repeat(BTN_RIGHT, 1)){
		first = false;
		//player settings
		player_h = 14;
		player_vy = 2;
		player_edge = 3;

		//opponent settings
		op_h = 14;
		op_vy = 2;
		op_edge = 3;

		//ball
		ball_vx = 2;
		ball_vy = 2;
	}

	//hard
	if(gb.buttons.repeat(BTN_DOWN, 1)){
		first = false;
		//player settings
		player_h = 14;
		player_vy = 1;
		player_edge = 3;

		//opponent settings
		op_h = 16;
		op_vy = 3;
		op_edge = 4;

		//ball
		ball_vx = 4;
		ball_vy = 3;
	}
}