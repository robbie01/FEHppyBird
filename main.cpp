#include "FEHLCD.h"
#include "FEHUtility.h"
#include "FEHRandom.h"

constexpr int XMIN = -2;
constexpr int XMAX = 284;
constexpr int YMIN = 0;
constexpr int YMAX = 226;

constexpr int GAPSIZE = 60;
constexpr int PIPEWIDTH = 20;

constexpr int BIRDHEIGHT = 20;
constexpr int BIRDWIDTH = 20;
constexpr int BIRDXPOS = 150;

constexpr int SCREENHEIGHT = 240;
constexpr int SCREENWIDTH = 320;

constexpr float GRAVITY = 0.2;
constexpr float FLAPVELOCITY = -3;

#define RANDOMCOLOR (((Random.RandInt()&0xFF)<<16)|((Random.RandInt()&0xFF)<<8)|((Random.RandInt()&0xFF)<<16))

class Renderable {
public:
	virtual void render() = 0;
};

class Pipe : public Renderable {
public:
	int gapheight, x;

	Pipe(int gapheight, int x) : gapheight(gapheight), x(x) {}

	void mod_x(int dx) {
		x += dx;
	}

	void render() {
		// Top pipe
		LCD.FillRectangle(x, 0, PIPEWIDTH, SCREENHEIGHT-gapheight-GAPSIZE/2);
		// Bottom pipe
		LCD.FillRectangle(x, SCREENHEIGHT-gapheight+GAPSIZE/2, PIPEWIDTH, gapheight-GAPSIZE/2);
	}
};

class Bird : public Renderable {
	float y, v = 0;
public:
	Bird(float y) : y(y) {}

	void update() {
		v += GRAVITY;
		y += v;
		if (y > SCREENHEIGHT-BIRDHEIGHT) {
			y = SCREENHEIGHT-BIRDHEIGHT;
			v = 0;
		} else if (y < 0) {
			y = 0;
			v = 0;
		}
	}

	void flap() {
		v = FLAPVELOCITY;
	}

	void render() {
		LCD.FillRectangle(BIRDXPOS, (int)y, 20, 20);
	}

	bool checkCollision(Pipe mypipe) {
		// Gapheight changes to top after robbies update
		// mypipe.x is left side of pipe
		// mypipe.gapheight is bottom edge of pipe
		// mypipe.gapheight + GAPSIZE is top edge of pipe
		if (
			(BIRDXPOS + BIRDWIDTH > mypipe.x && BIRDXPOS < mypipe.x + PIPEWIDTH) && 
			// Check lower, then upper collision
			(y + BIRDHEIGHT > SCREENHEIGHT - mypipe.gapheight || y < SCREENHEIGHT - GAPSIZE - mypipe.gapheight)
		) {
			return true;
		}
		return false;
	}
};

/*
 * Entry point to the application
 */
int main() {
    // Clear background
    LCD.SetBackgroundColor(BLACK);
    LCD.Clear();

    int x = XMIN, y = YMIN, dx = 1, dy = 1;

    float touchx, touchy;
    bool alive = true; 

    Pipe pipe(80, SCREENWIDTH);
    Bird bird(0);

    int waitingforup = 0;

    while (alive) {
	LCD.Clear();
	pipe.render();
	bird.render();
	// Check collision
	if (bird.checkCollision(pipe)) {
		alive = false;
	}
        LCD.WriteAt("DVD", x, y);
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
	pipe.mod_x(-1);
        //Sleep(20);
	x += dx, y += dy;
	if (x >= XMAX) {
		LCD.SetFontColor(RANDOMCOLOR);
		dx *= -1;
		x = XMAX;
	} else if (x <= XMIN) {
		LCD.SetFontColor(RANDOMCOLOR);
		dx *= -1;
		x = XMIN;
	}
	if (y >= YMAX) {
		LCD.SetFontColor(RANDOMCOLOR);
		dy *= -1;
		y = YMAX;
	} else if (y <= YMIN) {
		LCD.SetFontColor(RANDOMCOLOR);
		dy *= -1;
		y = YMIN;
	}
        // Never end
    }
    return 0;
}
