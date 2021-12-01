#include "FEHLCD.h"
#include "FEHUtility.h"

/*
 * Entry point to the application
 */
int main() {
    // Clear background
    LCD.SetBackgroundColor(BLACK);
    LCD.Clear();

    int x = -1, y = 0, dx = 1, dy = 1;

    while (1) {
	LCD.Clear();
        LCD.WriteAt("DVD", x, y);
        LCD.Update();
        //Sleep(20);
	x += dx, y += dy;
	if (x >= 284) {
		dx *= -1;
		x = 284;
	} else if (x <= -1) {
		dx *= -1;
		x = -1;
	}
	if (y >= 226) {
		dy *= -1;
		y = 226;
	} else if (y <= 0) {
		dy *= -1;
		y = 0;
	}
        // Never end
    }
    return 0;
}
