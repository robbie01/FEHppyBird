#include "FEHLCD.h"
#include "FEHUtility.h"

/*
 * Entry point to the application
 */
int main() {
    // Clear background
    LCD.SetBackgroundColor(BLACK);
    LCD.Clear();

    int x = 20, y = 20, dx = 2, dy = 2;

    while (1) {
	LCD.Clear();
        LCD.DrawCircle(x, y, 20);
        LCD.Update();
        //Sleep(20);
	x += dx, y += dy;
	if (x >= 299) {
		dx *= -1;
		x = 299;
	} else if (x <= 20) {
		dx *= -1;
		x = 20;
	}
	if (y >= 219) {
		dy *= -1;
		y = 219;
	} else if (y <= 20) {
		dy *= -1;
		y = 20;
	}
        // Never end
    }
    return 0;
}
