#include "FEHLCD.h"
#include "FEHRandom.h"
#include "FEHUtility.h"
#include "FEHSD.h"

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
	virtual void update() = 0;
	virtual void render() const = 0;
	virtual bool is_dead() const = 0;
};

class Pipe : public GameObject {
	bool dead = false;
public:
	int gapheight, x;

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
			LCD.SetFontColor(0x00AA00);
			LCD.FillRectangle(x, 0, PIPEWIDTH, gapheight);
			LCD.FillRectangle(x, gapheight + PIPEGAPSIZE, PIPEWIDTH, SCREENHEIGHT - gapheight - PIPEGAPSIZE);
		}
	}
};

class Bird : public GameObject {
	float y, v = 0;
	bool dead = false;
public:
	Bird(float y) : y(y) {}

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

	void feedCollision(Pipe mypipe) {
		if (
			// If front of bird is in front of the front of pipe and back of bird is behind the front of the pipe
			(BIRDXPOS + BIRDWIDTH > mypipe.x && BIRDXPOS < mypipe.x + PIPEWIDTH) &&
			// If top of the bird is above the upper gap or bottom of the bird is below the lower gap
			(y < mypipe.gapheight || y + BIRDHEIGHT > mypipe.gapheight + PIPEGAPSIZE)
		) {
			dead = true;
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

	Pipe pipe(Random.RandInt() % (PIPEGAPMAX+1), PIPEXMAX);
	Bird bird(0);

	int waitingforup = 0;

	while (!bird.is_dead()) {
		LCD.Clear();
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
		bird.update();
		pipe.update();
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
