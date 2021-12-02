#include "FEHLCD.h"
#include "FEHRandom.h"
#include "FEHUtility.h"
#include "FEHSD.h"

#include <vector>

constexpr int SCREENHEIGHT = 240;
constexpr int SCREENWIDTH = 320;

constexpr int PIPEGAPSIZE = 60;
constexpr int PIPEWIDTH = 20;
constexpr int PIPEVELOCITY = -2;
constexpr int PIPEXMIN = 0;
constexpr int PIPEXMAX = SCREENWIDTH - PIPEWIDTH;
constexpr int PIPEGAPMIN = 0;
constexpr int PIPEGAPMAX = SCREENHEIGHT - PIPEGAPSIZE;

constexpr int BIRDHEIGHT = 20;
constexpr int BIRDWIDTH = 20;
constexpr int BIRDXPOS = 150;

constexpr float BIRDGRAVITY = 0.2;
constexpr float BIRDFLAPVELOCITY = -3;
constexpr float BIRDYMIN = 0;
constexpr float BIRDYMAX = SCREENHEIGHT-BIRDHEIGHT-1;

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
		LCD.SetFontColor(0xFF0000);
		LCD.WriteAt(score, 0, 0);
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
		if (!dead && x <= PIPEXMAX) {
			LCD.SetFontColor(0x00AA00);
			LCD.FillRectangle(x, 0, PIPEWIDTH, gapheight);
			LCD.FillRectangle(x, gapheight + PIPEGAPSIZE, PIPEWIDTH, SCREENHEIGHT - gapheight - PIPEGAPSIZE);
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
		LCD.FillCircle(160, y + 10, 10);
	}

	void feedCollision(Pipe &pipe) {
		if (
			(y < pipe.gapheight || y + BIRDHEIGHT > pipe.gapheight + PIPEGAPSIZE) &&
			// If front of bird is in front of the front of pipe and back of bird is behind the front of the pipe
			(BIRDXPOS + BIRDWIDTH > pipe.x && BIRDXPOS < pipe.x + PIPEWIDTH)
			
		) {
			dead = true;
		} else if (!pipe.processed && pipe.x + PIPEWIDTH < BIRDXPOS) {
			pipe.processed = true;
			score.increment();
		}
	}
};

void display_image(const char *filename) {
	unsigned int color;
	FEHFile *imgfile = SD.FOpen(filename, "r");
	for (int y = 0; y < 240; ++y) {
		for (int x = 0; x < 320; ++x) {
			SD.FScanf(imgfile, "%u", &color);
			LCD.SetFontColor(color);
			LCD.DrawPixel(x, y);
		}
	}
	SD.FClose(imgfile);
}

/*
 * Entry point to the application
 */
int main() {
	LCD.SetBackgroundColor(BLACK);

	float touchx, touchy;

	ScoreCounter score(0);

	std::vector<Pipe> pipes;
	pipes.emplace_back(Random.RandInt() % (PIPEGAPMAX+1), PIPEXMAX);
	pipes.emplace_back(Random.RandInt() % (PIPEGAPMAX+1), PIPEXMAX+SCREENWIDTH/2);
	//pipes.emplace_back(Random.RandInt() % (PIPEGAPMAX+1), PIPEXMAX+SCREENWIDTH);

	//Pipe pipe(Random.RandInt() % (PIPEGAPMAX+1), PIPEXMAX);
	Bird bird(0, score);

	int waitingforup = 0;

	while (!bird.is_dead()) {
		LCD.Clear();
		for (Pipe &pipe : pipes) pipe.render();
		bird.render();
		score.render();
		LCD.Update();
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

	display_image("bob.txt");

	LCD.SetFontColor(0xFF0000);
	LCD.Write("You died!");

	while (1) LCD.Update();
	return 0;
}
