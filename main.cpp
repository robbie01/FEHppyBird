#include "FEHLCD.h"
#include "FEHRandom.h"
#include "FEHUtility.h"
#include "FEHSD.h"

#include <vector>

constexpr int SCREENHEIGHT = 240;
constexpr int SCREENWIDTH = 320;

constexpr int PIPEGAPSIZE = 40;
constexpr int PIPEWIDTH = 20;
constexpr int PIPEVELOCITY = -2;
constexpr int PIPEXMIN = 0;
constexpr int PIPEXMAX = SCREENWIDTH - PIPEWIDTH;
constexpr int PIPEGAPMIN = 0;
constexpr int PIPEGAPMAX = SCREENHEIGHT - PIPEGAPSIZE - 30;

constexpr int BIRDHEIGHT = 20;
constexpr int BIRDWIDTH = 20;
constexpr int BIRDXPOS = 75;

constexpr int SCOREXPOS = 150;
constexpr int SCOREYPOS = 30;

constexpr float BIRDGRAVITY = 0.2;
constexpr float BIRDFLAPVELOCITY = -3;
constexpr float BIRDYMIN = 0;
constexpr float BIRDYMAX = SCREENHEIGHT-BIRDHEIGHT-1;

constexpr int BACKDROPVELOCITY = -1;

using Image = unsigned int[SCREENWIDTH*SCREENHEIGHT];

#define RANDOMCOLOR (((Random.RandInt() & 0xFF) << 16) | ((Random.RandInt() & 0xFF) << 8) | ((Random.RandInt() & 0xFF) << 16))

// Parent class for everything on the screen, interactive or not
// update() updates internal state based on internal state only
// render() is for drawing to the LCD
// is_dead() signals to the "reaper" that the object should be destroyed.
//     It has additional significance for certain objects.
class GameObject {
public:
	GameObject() {}

	// Delete copy constructor and assignment operators
	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	// Undelete move constructor and assignment operator
	GameObject(GameObject&&) = default;
	GameObject& operator=(GameObject&&) = default;

	virtual void update() = 0;
	virtual void render() const = 0;
	virtual bool is_dead() const = 0;
};

class ScoreCounter : public GameObject {
	int score;
public:
	ScoreCounter(int score) : score(score) {}
	ScoreCounter() : score(0) {}

	void increment() {
		++score;
	}

	void update() {}
	bool is_dead() const { return false; }
	void render() const {
		LCD.SetFontColor(BLACK);
		LCD.WriteAt(score, SCOREXPOS+1, SCOREYPOS+1);
		LCD.WriteAt(score, SCOREXPOS+2, SCOREYPOS+2);
		LCD.SetFontColor(WHITE);
		LCD.WriteAt(score, SCOREXPOS, SCOREYPOS);
	}
};

class Pipe : public GameObject {
	bool dead = false;
public:
	int gapheight, x;
	bool processed = false;

	Pipe(int gapheight, int x) : gapheight(gapheight), x(x) {
		if (x < PIPEXMIN) this->x = PIPEXMIN;
		//else if (x > PIPEXMAX) this->x = PIPEXMAX;
	}

	void update() {
		if (!dead) {
			x += PIPEVELOCITY;
			if (x <= 0)
				dead = true;
		}
	}

	bool is_dead() const {
		return dead;
	}

	void render() const {
		if (!dead) {
			if (x <= PIPEXMAX) {
				LCD.SetFontColor(0x00AA00);
				// Draw top pipe
				LCD.FillRectangle(x, 0, PIPEWIDTH, gapheight);
				// Draw bottom pipe
				LCD.FillRectangle(x, gapheight + PIPEGAPSIZE, PIPEWIDTH, SCREENHEIGHT - gapheight - PIPEGAPSIZE);
				// Draw border
				LCD.SetFontColor(0);
				LCD.DrawRectangle(x, 0, PIPEWIDTH, gapheight);
				LCD.DrawRectangle(x, gapheight + PIPEGAPSIZE, PIPEWIDTH, SCREENHEIGHT - gapheight - PIPEGAPSIZE);
			} else if (x < SCREENWIDTH) {
				LCD.SetFontColor(0x00AA00);
				// Draw top pipe
				LCD.FillRectangle(x, 0, SCREENWIDTH-x, gapheight);
				// Draw bottom pipe
				LCD.FillRectangle(x, gapheight + PIPEGAPSIZE, SCREENWIDTH-x, SCREENHEIGHT - gapheight - PIPEGAPSIZE);
				// Draw border
				LCD.SetFontColor(0);
				LCD.DrawRectangle(x, 0, SCREENWIDTH-x, gapheight);
				LCD.DrawRectangle(x, gapheight + PIPEGAPSIZE, SCREENWIDTH-x, SCREENHEIGHT - gapheight - PIPEGAPSIZE);

			}
		}
	}
};

class Bird : public GameObject {
	float y, v = 0;
	bool dead = false;
	ScoreCounter &score;
public:
	Bird(float y, ScoreCounter &score) : y(y), score(score) {}

	void update() {
		v += BIRDGRAVITY;
		y += v;
		if (y >= BIRDYMAX) {
			y = BIRDYMAX;
			v = 0;
			dead = true;
		} else if (y <= BIRDYMIN) {
			y = BIRDYMIN;
			v = 0;
		}
	}

	bool is_dead() const {
		return dead;
	}

	void flap() {
		v = BIRDFLAPVELOCITY;
	}

	void render() const {
		LCD.SetFontColor(0xFFFF00);
		//LCD.FillRectangle(150, (int)y, 20, 20);
		LCD.FillCircle(BIRDXPOS+10, y + 10, 10);
		LCD.SetFontColor(0);
		LCD.DrawCircle(BIRDXPOS+10, y + 10, 10);
	}

	void feedCollision(Pipe &pipe) {
		if (
			(y < pipe.gapheight || y + BIRDHEIGHT > pipe.gapheight + PIPEGAPSIZE) &&
			// If front of bird is in front of the front of pipe and back of bird is behind the front of the pipe
			(BIRDXPOS + BIRDWIDTH > pipe.x && BIRDXPOS < pipe.x + PIPEWIDTH)
		) {
			dead = true;
		} else if (!pipe.processed && pipe.x + PIPEWIDTH/2 < BIRDXPOS + BIRDWIDTH/2) {
			pipe.processed = true;
			score.increment();
		}
	}
};

void read_image(const char *filename, Image img) {
	FEHFile *imgfile = SD.FOpen(filename, "r");
	for (int i = 0; i < SCREENWIDTH*SCREENHEIGHT; ++i) SD.FScanf(imgfile, "%u", img+i);
	SD.FClose(imgfile);
}

void display_image(const Image img, int x0, int y0) {
	for (int y = 0; y < 240; ++y) {
		for (int x = 0; x < 320; ++x) {
			LCD.SetFontColor(img[y*SCREENWIDTH+x]);
			LCD.DrawPixel(x+x0, y+y0);
		}
	}
}

void display_image(const char *filename, int x0, int y0) {
	Image img;
	read_image(filename, img);
	display_image(img, x0, y0);
}

class Backdrop : public GameObject {
	int x = 0;
	Image img;
public:
	Backdrop() {
		read_image("bliss.txt", img);
	}

	void update() {
		x = (x + BACKDROPVELOCITY) % SCREENWIDTH;
	}

	bool is_dead() const {
		return false;
	}

	void render() const {
		display_image(img, x, 0);
	}
};

enum NextState {
	MAIN_MENU,
	PLAY_GAME,
	STATS, 
	CREDITS,
	MANUAL
};
enum NextState credits()
{
	
		float touchx, touchy;

	LCD.SetBackgroundColor(STEELBLUE);
	LCD.Clear();
	LCD.SetFontColor(RED);

	LCD.WriteAt("Sammy Aidja", 50, 100);
	LCD.WriteAt("Robbie Langer", 50, 150);
	LCD.WriteAt("Luke Weitler", 50, 200);

	LCD.SetFontColor(0xFFFFFF);
	LCD.WriteAt("Return to menu", 14, 60);
	LCD.Update();
	bool selected = false;
	enum NextState next_state;

	while (!selected) {
		while (!LCD.Touch(&touchx, &touchy));
		if (touchx >= 12 && touchx < 12+174 && touchy >= 56 && touchy < 56+22) {
			selected = true;
			next_state = MAIN_MENU;
		}
	}

	return next_state;

}


enum NextState main_menu() {
	// TODO: add actual menu

	LCD.SetBackgroundColor(STEELBLUE);
	LCD.Clear();
	LCD.FillCircle(25, 150, 25);
	LCD.FillCircle(45, 130, 15);
	LCD.FillCircle(75, 120, 25);
	LCD.FillCircle(100, 150, 40);	
	LCD.FillCircle(150, 130, 25);
	LCD.FillCircle(190, 125, 30);
	LCD.FillCircle(230, 120, 40);
	LCD.FillCircle(275, 120, 25);
	LCD.FillCircle(300, 130, 30);
	LCD.FillRectangle(0, 130, 320, 110);


	LCD.SetFontColor(BLACK);
	LCD.WriteAt("Flappy Bird", 91, 41);
	LCD.WriteAt("Flappy Bird", 89, 39);
	LCD.SetFontColor(WHITE);
	LCD.WriteAt("Flappy Bird", 90,40);

	LCD.SetFontColor(ORANGERED);
	LCD.FillRectangle(38, 125, 62, 35);
	LCD.SetFontColor(WHITE);
	LCD.DrawRectangle(37,124, 63, 36);
	LCD.SetFontColor(BLACK);
	LCD.DrawRectangle(36, 123, 65, 38);

	LCD.SetFontColor(ORANGERED);
	LCD.FillRectangle(200, 125, 62, 35);
	LCD.SetFontColor(WHITE);
	LCD.DrawRectangle(199,124, 63, 36);
	LCD.SetFontColor(BLACK);
	LCD.DrawRectangle(198, 123, 65, 38);

	LCD.SetFontColor(ORANGERED);
	LCD.FillRectangle(190, 190, 86, 35);
	LCD.SetFontColor(WHITE);
	LCD.DrawRectangle(189,189, 87, 36);
	LCD.SetFontColor(BLACK);
	LCD.DrawRectangle(188, 188, 89, 38);

	LCD.SetFontColor(ORANGERED);
	LCD.FillRectangle(28, 190, 86, 35);
	LCD.SetFontColor(WHITE);
	LCD.DrawRectangle(27,189, 87, 36);
	LCD.SetFontColor(BLACK);
	LCD.DrawRectangle(26, 188, 89, 38);

	LCD.WriteAt("Start", 38, 136);
	LCD.WriteAt("Stats", 200, 136);
	LCD.WriteAt("Credits", 190, 200);
	LCD.WriteAt("Manual", 33, 200);
	LCD.Update();

	float touchx, touchy;
	bool selected = false;
	enum NextState next_state;

	while(!selected)
	{
		while(!LCD.Touch(&touchx, &touchy));

			if(touchx>=35 && touchx<=101 && touchy>=123 && touchy<=161)
			{
				selected=true;
				printf("start");
				next_state =PLAY_GAME;
			}

			else if(touchx>=197 && touchx<=263 && touchy>=123 && touchy<=161)
			{
				printf("Stats");
				selected=true;
				next_state =STATS;
			}

			else if(touchx>=187 && touchx<=276 && touchy>=187 && touchy<=225)
			{
				printf("Credits");
				selected=true;
				next_state=CREDITS;
				
			}

			else if(touchx>=25 && touchx<=115 && touchy>=187 && touchy<=226)
			{
				printf("manual");
				selected=true;
				next_state=MANUAL;
			}
	}
	return next_state;
	/*
	LCD.SetFontColor(0xFF0000);
	LCD.WriteAt("You died!", 1, 1);

	LCD.SetFontColor(0xFFFFFF);
	LCD.WriteAt("Restart", 14, 37);
	LCD.WriteAt("Return to menu", 14, 60);

	bool selected = false;
	enum NextState next_state;

	while (!selected) {
		while (!LCD.Touch(&touchx, &touchy));
		if (touchx >= 12 && touchx < 12+90 && touchy >= 33 && touchy < 33+22) {
			selected = true;
			next_state = PLAY_GAME;
		} else if (touchx >= 12 && touchx < 12+174 && touchy >= 56 && touchy < 56+22) {
			selected = true;
			next_state = MAIN_MENU;
		}
		
	*/
}
enum NextState manual()
{
		float touchx, touchy;

	LCD.SetBackgroundColor(BLACK);
	LCD.Clear();
	LCD.SetFontColor(RED);

	LCD.WriteAt("Manual", 50, 200);
	
	LCD.SetFontColor(0xFFFFFF);
	LCD.WriteAt("Return to menu", 14, 60);
	LCD.Update();
	bool selected = false;
	enum NextState next_state;

	while (!selected) {
		while (!LCD.Touch(&touchx, &touchy));
		if (touchx >= 12 && touchx < 12+174 && touchy >= 56 && touchy < 56+22) {
			selected = true;
			next_state = MAIN_MENU;
		}
	}

	return next_state;

}
enum NextState stats()
{
		float touchx, touchy;


	LCD.SetBackgroundColor(BLACK);
	LCD.Clear();
	LCD.SetFontColor(RED);
	LCD.WriteAt("STATS", 50, 200);	
	LCD.SetFontColor(0xFFFFFF);
	LCD.WriteAt("Return to menu", 14, 60);
	LCD.Update();
	bool selected = false;
	enum NextState next_state;

	while (!selected) {
		while (!LCD.Touch(&touchx, &touchy));
		if (touchx >= 12 && touchx < 12+174 && touchy >= 56 && touchy < 56+22) {
			selected = true;
			next_state = MAIN_MENU;
		}
	}

	return next_state;



}
enum NextState play_game() {
	LCD.SetBackgroundColor(BLACK);

	float touchx, touchy;

	ScoreCounter score(0);
	Backdrop backdrop;

	std::vector<Pipe> pipes;
	pipes.emplace_back(Random.RandInt() % (PIPEGAPMAX+1), PIPEXMAX);
	pipes.emplace_back(Random.RandInt() % (PIPEGAPMAX+1), PIPEXMAX+SCREENWIDTH/2);
	//pipes.emplace_back(Random.RandInt() % (PIPEGAPMAX+1), PIPEXMAX+SCREENWIDTH);

	//Pipe pipe(Random.RandInt() % (PIPEGAPMAX+1), PIPEXMAX);
	Bird bird(0, score);

	int waitingforup = 0;

	while (!bird.is_dead()) {
		LCD.Clear();
		backdrop.render();
		for (Pipe &pipe : pipes) pipe.render();
		bird.render();
		score.render();
		LCD.Update();
		backdrop.update();
		if (LCD.Touch(&touchx, &touchy)) {
			if (!waitingforup) {
				bird.flap();
				waitingforup = 1;
			}
		} else {
			waitingforup = 0;
		}
		bird.update();
		for (Pipe &pipe : pipes) {
			pipe.update();
			bird.feedCollision(pipe);
		}
		for (auto it = pipes.begin(); it != pipes.end(); ++it) {
			if (it->is_dead()) {
				pipes.erase(it);
				pipes.emplace_back(Random.RandInt() % (PIPEGAPMAX+1), PIPEXMAX+PIPEWIDTH);
				break;
			}
		}
		//Sleep(20);
		// Never end
	}

	LCD.SetBackgroundColor(BLACK);
	LCD.Clear();

	display_image("bob.txt", 0, 0);

	LCD.SetFontColor(0);
	LCD.FillRectangle(12, 33, 90, 22);
	LCD.FillRectangle(12, 56, 174, 22);

	LCD.SetFontColor(0xFFFFFF);
	LCD.DrawRectangle(12, 33, 90, 22);
	LCD.DrawRectangle(12, 56, 174, 22);

	LCD.SetFontColor(0xFF0000);
	LCD.WriteAt("You died!", 1, 1);

	LCD.SetFontColor(0xFFFFFF);
	LCD.WriteAt("Restart", 14, 37);
	LCD.WriteAt("Return to menu", 14, 60);

	bool selected = false;
	enum NextState next_state;

	while (!selected) {
		while (!LCD.Touch(&touchx, &touchy));
		if (touchx >= 12 && touchx < 12+90 && touchy >= 33 && touchy < 33+22) {
			selected = true;
			next_state = PLAY_GAME;
		} else if (touchx >= 12 && touchx < 12+174 && touchy >= 56 && touchy < 56+22) {
			selected = true;
			next_state = MAIN_MENU;
		}
	}

	return next_state;
}

/*
 * Entry point to the application
 */
int main() {
	enum NextState next_state = MAIN_MENU;
	while (true) {
		switch (next_state) {
		case MAIN_MENU:
			next_state = main_menu();
			break;
		case PLAY_GAME:
			next_state = play_game();
			break;
		case MANUAL:
			next_state = manual();
			break;
		case STATS:
			next_state = stats();
			break;
		case CREDITS:
			next_state = credits();
			break;
		}
	}
	return 0;
}
