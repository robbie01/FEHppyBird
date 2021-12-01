#include "FEHLCD.h"
#include "FEHUtility.h"

constexpr int XMIN = -2;
constexpr int XMAX = 284;
constexpr int YMIN = 0;
constexpr int YMAX = 226;

/*
 * Entry point to the application
 */
int main() {
    // Clear background
    LCD.SetBackgroundColor(BLACK);
    LCD.Clear();

    int x = XMIN, y = YMIN, dx = 1, dy = 1;

    while (1) {
	LCD.Clear();
        LCD.WriteAt("DVD", x, y);
        LCD.Update();
        //Sleep(20);
	x += dx, y += dy;
	if (x >= XMAX) {
		dx *= -1;
		x = XMAX;
	} else if (x <= XMIN) {
		dx *= -1;
		x = XMIN;
	}
	if (y >= YMAX) {
		dy *= -1;
		y = YMAX;
	} else if (y <= YMIN) {
		dy *= -1;
		y = YMIN;
	}
        // Never end
    }
    return 0;
}
