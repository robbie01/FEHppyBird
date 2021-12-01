#include "FEHLCD.h"
#include "FEHUtility.h"
#include "FEHRandom.h"

constexpr int XMIN = -2;
constexpr int XMAX = 284;
constexpr int YMIN = 0;
constexpr int YMAX = 226;

constexpr int GAPSIZE = 60;
constexpr int PIPEWIDTH = 20;

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
	int gapheight, x;
public:
	Pipe(int gapheight, int x) : gapheight(gapheight), x(x) {}

	void mod_x(int dx) {
		x += dx;
	}

	void render() {
		LCD.FillRectangle(x, 0, PIPEWIDTH, SCREENHEIGHT-gapheight-GAPSIZE/2);
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
		if (y > SCREENHEIGHT-20) {
			y = SCREENHEIGHT-20;
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
		LCD.FillRectangle(150, (int)y, 20, 20);
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

    Pipe pipe(80, SCREENWIDTH);
    Bird bird(0);

    int waitingforup = 0;

    while (1) {
	LCD.Clear();
	pipe.render();
	bird.render();
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
