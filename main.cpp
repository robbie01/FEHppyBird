#include "FEHLCD.h"
#include "FEHRandom.h"
#include "FEHUtility.h"

constexpr int DVDXMIN = -2;
constexpr int DVDXMAX = 284;
constexpr int DVDYMIN = 0;
constexpr int DVDYMAX = 226;
constexpr int DVDDXI = 1;
constexpr int DVDDYI = 1;

constexpr int PIPEGAPSIZE = 60;
constexpr int PIPEWIDTH = 20;
constexpr int PIPEVELOCITY = -2;

constexpr int BIRDHEIGHT = 20;
constexpr int BIRDWIDTH = 20;
constexpr int BIRDXPOS = 150;

constexpr int SCREENHEIGHT = 240;
constexpr int SCREENWIDTH = 320;

constexpr float BIRDGRAVITY = 0.2;
constexpr float BIRDFLAPVELOCITY = -3;

#define RANDOMCOLOR (((Random.RandInt() & 0xFF) << 16) | ((Random.RandInt() & 0xFF) << 8) | ((Random.RandInt() & 0xFF) << 16))

class GameObject {
public:
	virtual void update() = 0;
	virtual void render() const = 0;
	virtual int is_dead() const = 0;
};

class Pipe : public GameObject {
	int dead = 0;
public:
	int gapheight, x;

	Pipe(int gapheight, int x) : gapheight(gapheight), x(x) {}

	void update() {
		if (!dead) {
			x += PIPEVELOCITY;
			if (x <= 0)
				dead = 1;
		}
	}

	int is_dead() const {
		return dead;
	}

	void render() const {
		if (!dead) {
			LCD.SetFontColor(0x00AA00);
			LCD.FillRectangle(x, 0, PIPEWIDTH, gapheight);
			LCD.FillRectangle(x, gapheight + PIPEGAPSIZE, PIPEWIDTH, SCREENHEIGHT - gapheight - PIPEGAPSIZE);
		}
	}
};

class Bird : public GameObject {
	float y, v = 0;
	int dead = 0;
public:
	Bird(float y) : y(y) {}

	void update() {
		v += BIRDGRAVITY;
		y += v;
		if (y > SCREENHEIGHT-BIRDHEIGHT-1) {
			y = SCREENHEIGHT-BIRDHEIGHT-1;
			v = 0;
			dead = 1;
		} else if (y < 0) {
			y = 0;
			v = 0;
		}
	}

	int is_dead() const {
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

	void feedCollision(Pipe mypipe) {
		if (
			(BIRDXPOS + BIRDWIDTH > mypipe.x && BIRDXPOS < mypipe.x + PIPEWIDTH) &&
			(y < mypipe.gapheight || y + BIRDHEIGHT > mypipe.gapheight + PIPEGAPSIZE)
		) {
			dead = 1;
		}
	}
};

class DVD : public GameObject {
	int x = DVDXMIN, y = DVDYMIN, dx = DVDDXI, dy = DVDDYI;
	unsigned int color;
public:
	DVD() : color(RANDOMCOLOR) {}

	void update() {
		x += dx, y += dy;
		if (x >= DVDXMAX) {
			color = RANDOMCOLOR;
			dx *= -1;
			x = DVDXMAX;
		} else if (x <= DVDXMIN) {
			color = RANDOMCOLOR;
			dx *= -1;
			x = DVDXMIN;
		}
		if (y >= DVDYMAX) {
			color = RANDOMCOLOR;
			dy *= -1;
			y = DVDYMAX;
		} else if (y <= DVDYMIN) {
			color = RANDOMCOLOR;
			dy *= -1;
			y = DVDYMIN;
		}
	}

	int is_dead() const {
		return 0;
	}

	void render() const {
		LCD.SetFontColor(color);
		LCD.WriteAt("DVD", x, y);
	}
};

/*
 * Entry point to the application
 */
int main() {
	// Clear background
	LCD.SetBackgroundColor(BLACK);
	LCD.Clear();

	float touchx, touchy;

	Pipe pipe(Random.RandInt() % (SCREENHEIGHT - PIPEGAPSIZE), SCREENWIDTH - PIPEWIDTH);
	Bird bird(0);
	DVD dvd;

	int waitingforup = 0;

	while (!bird.is_dead()) {
		LCD.Clear();
		dvd.render();
		pipe.render();
		bird.render();
		bird.feedCollision(pipe);
		LCD.Update();
		if (LCD.Touch(&touchx, &touchy)) {
			if (!waitingforup) {
				bird.flap();
				waitingforup = 1;
			}
		} else {
			waitingforup = 0;
		}
		dvd.update();
		bird.update();
		pipe.update();
		//Sleep(20);
		// Never end
	}

	LCD.Clear();
	LCD.SetFontColor(0xFF0000);
	LCD.Write("You died!");

	while (1) LCD.Update();
	return 0;
}
