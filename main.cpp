#include "FEHLCD.h"
#include "FEHRandom.h"
#include "FEHSD.h"
#include "FEHUtility.h"

#include <vector>

constexpr int SCREENHEIGHT = 240;
constexpr int SCREENWIDTH = 320;

constexpr int PIPEGAPSIZE = 55;
constexpr int PIPEWIDTH = 20;
constexpr int PIPEVELOCITY = -2;
constexpr int PIPEXMIN = 0;
constexpr int PIPEXMAX = SCREENWIDTH - PIPEWIDTH;
constexpr int PIPEGAPMIN = 0;
constexpr int PIPEGAPMAX = SCREENHEIGHT - PIPEGAPSIZE - 30;

constexpr int BIRDHEIGHT = 20;
constexpr int BIRDWIDTH = 20;
constexpr int BIRDYSTART = 70;
constexpr int BIRDXPOS = 75;

constexpr int SCOREXPOS = 150;
constexpr int SCOREYPOS = 30;

constexpr float BIRDGRAVITY = 0.6;
constexpr float BIRDFLAPVELOCITY = -6;
constexpr float BIRDYMIN = 0;
constexpr float BIRDYMAX = SCREENHEIGHT - BIRDHEIGHT - 1;

constexpr int BACKDROPVELOCITY = -1;

using Image = unsigned int[SCREENWIDTH * SCREENHEIGHT];

#define RANDOMCOLOR (((Random.RandInt() & 0xFF) << 16) | ((Random.RandInt() & 0xFF) << 8) | ((Random.RandInt() & 0xFF) << 16))

// Parent class for everything on the screen, interactive or not
// update() updates internal state based on internal state only
// render() is for drawing to the LCD
// is_dead() signals to the "reaper" that the object should be destroyed.
//     (This didn't end up actually happening in practice; the game was too simple for such reaping to be necessary.)
//     It has additional significance for certain objects.
//robbie
class GameObject {
public:
	GameObject() {}

	// Delete copy constructor and assignment operators
	GameObject(const GameObject &) = delete;
	GameObject &operator=(const GameObject &) = delete;

	// Undelete move constructor and assignment operator
	GameObject(GameObject &&) = default;
	GameObject &operator=(GameObject &&) = default;

	virtual void update() = 0;
	virtual void render() const = 0;
	virtual bool is_dead() const = 0;
};

// Class that manages the player's score
// Private variable _score() holds the score and prevents it from being accessed by outside code
// Can be instantiated with a defined starting score or set _score to 0 if no score is passed
// increment() increases the value of _score by 1
// the update() and is_dead() methods are left empty in this class because it is a GameObject that does not move or get killed
// render() prints the score to the screen/HUD
// score() gets the value of _score
//robbie
class ScoreCounter : public GameObject {
	int _score;
public:
	ScoreCounter(int score) : _score(score) {}
	ScoreCounter() : _score(0) {}

	// Increment the score in a nice OO way; this makes modification of the score more traceable.
	void increment() {
		++_score;
	}

	// No frame-by-frame updates needed
	void update() {}

	// Immortal
	bool is_dead() const { return false; }

	// Throw it on the screen
	void render() const {
		LCD.SetFontColor(BLACK);
		LCD.WriteAt(_score, SCOREXPOS + 1, SCOREYPOS + 1);
		LCD.WriteAt(_score, SCOREXPOS + 2, SCOREYPOS + 2);
		LCD.SetFontColor(WHITE);
		LCD.WriteAt(_score, SCOREXPOS, SCOREYPOS);
	}

	// Get the score in a nice OO way. Makes the value itself read-only so that it can only be modified through increment().
	int score() const {
		return _score;
	}
};

// Pipe Class stores and manages information about each pipe
// dead tells the program if the pipe has reached the end of the screen and is killed
// gapheight holds the distance from the top of the screen to the top of the gap
// x holds the x position of the pipe
// processed tells the program if the pipe has been updated or not
// The constructor takes parameters gapheight and x, which set the x position and gapheight of the pipe as described previously
// update() moves the pipe leftwards as long as the pipe is not dead and kills it if it cannot move left
// is_dead() returns the value of dead
// render() renders a visual representation of the pipe to the screen
//robbie
class Pipe : public GameObject {
	bool dead = false;
public:
	int gapheight, x;
	bool processed = false;

	// Clamp the horizontal position for whatever reason. Probably not needed but it doesn't hurt
	Pipe(int gapheight, int x) : gapheight(gapheight), x(x) {
		if (x < PIPEXMIN)
			this->x = PIPEXMIN;
		//else if (x > PIPEXMAX) this->x = PIPEXMAX;
	}

	// Move the pipe, and kill it at the edge.
	void update() {
		if (!dead) {
			x += PIPEVELOCITY;
			if (x <= 0)
				dead = true;
		}
	}

	// Get whether the pipe has been killed
	bool is_dead() const {
		return dead;
	}

	void render() const {
		if (!dead) { // Only render if the pipe hasn't been killed
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
			} else if (x < SCREENWIDTH) { // Handle the pipe not having made it to the left edge yet, draw partial pipe
				LCD.SetFontColor(0x00AA00);
				// Draw top pipe
				LCD.FillRectangle(x, 0, SCREENWIDTH - x, gapheight);
				// Draw bottom pipe
				LCD.FillRectangle(x, gapheight + PIPEGAPSIZE, SCREENWIDTH - x, SCREENHEIGHT - gapheight - PIPEGAPSIZE);
				// Draw border
				LCD.SetFontColor(0);
				LCD.DrawRectangle(x, 0, SCREENWIDTH - x, gapheight);
				LCD.DrawRectangle(x, gapheight + PIPEGAPSIZE, SCREENWIDTH - x, SCREENHEIGHT - gapheight - PIPEGAPSIZE);
			}
		}
	}
};

// Bird class holds information about the bird
// y is used to hold the vertical position of the bird
// v is the velocity of the bird
// dead states whether or not the bird should be rendered or not
// ScoreCounter &score allows the bird to set the score from the bird methods
// Constructor takes the initial position y and an instance of scorecounter as parameters
// update() updates the position of the bird by applying gravity and it's velocity to it's position
// is_dead() returns the value of dead
// flap() sets the birds velocity to the constant BIRDFLAPVELOCITY
// render() prints a visual representation of the bird to the LCD
// feedCollision() determines if the bird collided with the pipe given in the parameters
//robbie
class Bird : public GameObject {
	float y, v = 0;
	bool dead = false;
	ScoreCounter &score;
public:
	// New bird just dropped
	Bird(float y, ScoreCounter &score) : y(y), score(score) {}

	void update() { // Handle a change in the time domain
		// The below two lines implement the semi-implicit Euler method (also known as the symplectic Euler method)
		v += BIRDGRAVITY;
		y += v;
		if (y >= BIRDYMAX) { // Handle the bird hitting the ground
			y = BIRDYMAX;
			v = 0;
			dead = true;
		} else if (y <= BIRDYMIN) { // Prevent the bird from going off the top of the screen
			y = BIRDYMIN;
			v = 0;
		}
	}

	bool is_dead() const {
		return dead;
	}

	void flap() { // Set the bird's velocity to a constant upward value on flap.
		v = BIRDFLAPVELOCITY;
	}

	void render() const { // This poor bird is just a circle. Looks like a coin from a platformer
		LCD.SetFontColor(0xFFFF00);
		//LCD.FillRectangle(150, (int)y, 20, 20);
		LCD.FillCircle(BIRDXPOS + 10, y + 10, 10);
		LCD.SetFontColor(0);
		LCD.DrawCircle(BIRDXPOS + 10, y + 10, 10);
	}
	//luke
	void feedCollision(Pipe &pipe) {
		if (
		    (y < pipe.gapheight || y + BIRDHEIGHT > pipe.gapheight + PIPEGAPSIZE) &&
		    // If front of bird is in front of the front of pipe and back of bird is behind the front of the pipe
		    (BIRDXPOS + BIRDWIDTH > pipe.x && BIRDXPOS < pipe.x + PIPEWIDTH)) {
			dead = true;
		} else if (!pipe.processed && pipe.x + PIPEWIDTH / 2 < BIRDXPOS + BIRDWIDTH / 2) {
			// Handle the bird successfully navigating the pipe
			pipe.processed = true;
			score.increment();
		}
	}
};
//robbie
// Read an image from a file into memory. The format of the file is to have a 32-bit integer on each line, each representing the color of a single pixel.
void read_image(const char *filename, Image img) {
	FEHFile *imgfile = SD.FOpen(filename, "r");
	for (int i = 0; i < SCREENWIDTH * SCREENHEIGHT; ++i)
		SD.FScanf(imgfile, "%u", img + i);
	SD.FClose(imgfile);
}

// Display an image from memory, possibly with an offset.
void display_image(const Image img, int x0 = 0, int y0 = 0) {
	for (int y = 0; y < 240; ++y) {
		for (int x = 0; x < 320; ++x) {
			LCD.SetFontColor(img[y * SCREENWIDTH + x]);
			LCD.DrawPixel(x + x0, y + y0);
		}
	}
}

// Simple wrapper around the above two functions for images that don't need to be redrawn.
void display_image(const char *filename, int x0 = 0, int y0 = 0) {
	Image img;
	read_image(filename, img);
	display_image(img, x0, y0);
}

// Backdrop class holds and renders the image shown in the background during the game
// x holds the horizontal position of the backdrop
// img holds the Image content
// The constructor reads the image "img/bliss.txt" to img
// update() updates the position of the backdrop
// is_dead() returns false because the backdrop doesn't die
// render() displays the image
//robbie
class Backdrop : public GameObject {
	int x = 0;
	Image img;
public:
	// Maybe the backdrop shouldn't be hardcoded in? I don't see why anyone wouldn't want this background tho
	Backdrop() {
		read_image("img/bliss.txt", img);
	}

	// The modulo isn't strictly necessary because the LCD library itself does that, but it's good at preventing int overflows
	void update() {
		x = (x + BACKDROPVELOCITY) % SCREENWIDTH;
	}

	// What is dead may never die
	bool is_dead() const {
		return false;
	}

	// Render!
	void render() const {
		display_image(img, x, 0);
	}
};

// This game is implemented as a half-hearted state machine so that screens can be transitioned between easily without having to worry about a stack overflow.
enum NextState {
	MAIN_MENU,
	PLAY_GAME,
	STATS,
	CREDITS,
	MANUAL,
	QUIT
};

// Prints information about the faces behind the game. Give them the credit they deserve.
//Sammy
enum NextState credits() {
	float touchx, touchy;

	LCD.SetBackgroundColor(BLACK);
	LCD.Clear();
	LCD.SetFontColor(WHITE);

	LCD.WriteAt("Staff:", 25, 50);
	LCD.WriteAt("Sammy Aidja", 50, 75);
	LCD.WriteAt("Robbie Langer", 50, 100);
	LCD.WriteAt("Luke Weiler", 50, 125);
	LCD.WriteAt("Original Work:", 25, 150);
	LCD.WriteAt("\"Flappy Bird\"", 50, 175);
	LCD.WriteAt("by Dong Nguyen", 50, 200); // dong y u take flappy bird off of app store

	LCD.SetFontColor(0xFFFFFF);
	LCD.WriteAt("Return to menu", 14, 20);
	LCD.DrawRectangle(12, 16, 182, 24);
	LCD.Update();
	bool selected = false;
	enum NextState next_state;

	while (!selected) {
		while (!LCD.Touch(&touchx, &touchy))
			;
		if (touchx >= 12 && touchx < 12 + 182 && touchy >= 16 && touchy < 16 + 24) {
			selected = true;
			next_state = MAIN_MENU;
		}
	}

	return next_state;
}

// Render the menu and listen for clicks
//Sammy
enum NextState main_menu() {
	LCD.SetBackgroundColor(STEELBLUE);
	LCD.Clear();
	LCD.SetFontColor(WHITE);
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

	// Add a pseudo font shadow
	LCD.SetFontColor(BLACK);
	LCD.WriteAt("Flappy Bird", 91, 41);
	LCD.WriteAt("Flappy Bird", 89, 39);
	LCD.SetFontColor(WHITE);
	LCD.WriteAt("Flappy Bird", 90, 40);

	LCD.SetFontColor(ORANGERED);
	LCD.FillRectangle(38, 125, 62, 35);
	LCD.SetFontColor(WHITE);
	LCD.DrawRectangle(37, 124, 63, 36);
	LCD.SetFontColor(BLACK);
	LCD.DrawRectangle(36, 123, 65, 38);

	LCD.SetFontColor(ORANGERED);
	LCD.FillRectangle(200, 125, 62, 35);
	LCD.SetFontColor(WHITE);
	LCD.DrawRectangle(199, 124, 63, 36);
	LCD.SetFontColor(BLACK);
	LCD.DrawRectangle(198, 123, 65, 38);

	LCD.SetFontColor(ORANGERED);
	LCD.FillRectangle(190, 190, 86, 35);
	LCD.SetFontColor(WHITE);
	LCD.DrawRectangle(189, 189, 87, 36);
	LCD.SetFontColor(BLACK);
	LCD.DrawRectangle(188, 188, 89, 38);

	LCD.SetFontColor(ORANGERED);
	LCD.FillRectangle(28, 190, 86, 35);
	LCD.SetFontColor(WHITE);
	LCD.DrawRectangle(27, 189, 87, 36);
	LCD.SetFontColor(BLACK);
	LCD.DrawRectangle(26, 188, 89, 38);

	LCD.SetFontColor(ORANGERED);
	LCD.FillRectangle(121, 155, 60, 33);
	LCD.SetFontColor(WHITE);
	LCD.DrawRectangle(120, 154, 61, 34);
	LCD.SetFontColor(BLACK);
	LCD.DrawRectangle(119, 153, 63, 36);

	LCD.SetFontColor(BLACK);
	LCD.WriteAt("Start", 38, 136);
	LCD.WriteAt("Stats", 200, 136);
	LCD.WriteAt("Credits", 190, 200);
	LCD.WriteAt("Manual", 33, 200);
	LCD.WriteAt("Quit", 126, 165);
	LCD.Update();

	float touchx, touchy;
	bool selected = false;
	enum NextState next_state;

	while (!selected) {
		// Handle all touches, not just valid ones. Don't do anything with the invalid touches though.
		while (!LCD.Touch(&touchx, &touchy))
			;

		if (touchx >= 35 && touchx <= 101 && touchy >= 123 && touchy <= 161) {
			selected = true;
			next_state = PLAY_GAME;
		} else if (touchx >= 197 && touchx <= 263 && touchy >= 123 && touchy <= 161) {
			selected = true;
			next_state = STATS;
		} else if (touchx >= 187 && touchx <= 276 && touchy >= 187 && touchy <= 225) {
			selected = true;
			next_state = CREDITS;
		} else if (touchx >= 25 && touchx <= 115 && touchy >= 187 && touchy <= 226) {
			selected = true;
			next_state = MANUAL;
		} else if (touchx >= 119 && touchx < 119 + 63 && touchy >= 153 && touchy < 153 + 36) {
			selected = true;
			next_state = QUIT;
		}
	}
	return next_state;
}

// Shows instructions for how to play the game and listens to see if you click to go back to the menu
//Sammy
enum NextState manual() {
	float touchx, touchy;

	LCD.SetBackgroundColor(BLACK);
	LCD.Clear();
	LCD.SetFontColor(WHITE);

	LCD.WriteAt("Manual:", 20, 100);
	LCD.WriteAt("* Tap/click to flap", 20, 120);
	LCD.WriteAt("* Fly through the gaps ", 20, 140);
	LCD.WriteAt("  in the pipes to score.", 20, 160);
	LCD.WriteAt("* If you hit a pipe or ", 20, 180);
	LCD.WriteAt("  the ground, the game", 20, 200);
	LCD.WriteAt("  ends", 20, 220);

	LCD.SetFontColor(WHITE);
	LCD.WriteAt("Return to menu", 14, 60);
	LCD.DrawRectangle(12, 55, 175, 25);
	LCD.Update();
	bool selected = false;
	enum NextState next_state;

	while (!selected) {
		while (!LCD.Touch(&touchx, &touchy))
			;
		if (touchx >= 12 && touchx < 12 + 175 && touchy >= 55 && touchy < 55 + 25) {
			selected = true;
			next_state = MAIN_MENU;
		}
	}

	return next_state;
}

// bubblesort j4f
template <class T>
void bubblesort(std::vector<T> &x) {
	bool sorted = false;
	while (!sorted) {
		sorted = true;
		for (auto it = x.begin(); it != x.end() - 1; ++it) {
			if (*it > *(it + 1)) {
				T temp = *it;
				*it = *(it + 1);
				*(it + 1) = temp;
				sorted = false;
			}
		}
	}
}

// Finds the minimum of two values
int min(int a, int b) {
	return (a < b) ? a : b;
}

// Displays the top 5 scores; less if there aren't 5 scores yet. Listens for clicks to go back to main menu
//Sammy
enum NextState stats() {
	float touchx, touchy;

	FEHFile *data;
	std::vector<int> count;

	data = SD.FOpen("High Scores.txt", "r");
	if (!data)
		goto badfile;

	int status, x;

	while (true) {
		status = SD.FScanf(data, "%i", &x);
		if (status == EOF)
			break; // inelegant way of implementing a "Dahl loop"
		count.push_back(x);
	}

	SD.FClose(data);

	bubblesort(count);

badfile:

	LCD.SetBackgroundColor(BLACK);
	LCD.Clear();
	LCD.SetFontColor(WHITE);
	LCD.WriteAt("High Scores", 98, 50);

	for(int i=1; i<min(count.size(), 5)+1; i++) {
		LCD.WriteAt(i, 20, 60 + 30*i);
		LCD.WriteAt(")", 30, 60 + 30*i);
		LCD.WriteAt(count[count.size() - i], 50, 60 + 30*i);
	}

	LCD.SetFontColor(0xFFFFFF);
	LCD.WriteAt("Return to menu", 20, 20);
	LCD.DrawRectangle(18, 16, 177, 22);
	LCD.Update();
	bool selected = false;
	enum NextState next_state;

	while (!selected) {
		while (!LCD.Touch(&touchx, &touchy))
			;
		if (touchx >= 18 && touchx < 18 + 177 && touchy >= 16 && touchy < 16 + 22) {
			selected = true;
			next_state = MAIN_MENU;
		}
	}

	return next_state;
}

// Contains code for instantiating game objects and running a loop that updates and renders objects and listens for clicks
//Robbie
enum NextState play_game() {
	LCD.SetBackgroundColor(BLACK);
	LCD.Clear();

	LCD.SetFontColor(0xFFFFFF);
	LCD.WriteRC("Loading...", 0, 0);
	LCD.Update();

	float touchx, touchy;

	ScoreCounter score(0);
	Backdrop backdrop;

	std::vector<Pipe> pipes; // Apologies for the std. I'm not creative to work around this myself, so I hope that vectors are possible on the Proteus.
	pipes.emplace_back(Random.RandInt() % (PIPEGAPMAX + 1), PIPEXMAX);
	pipes.emplace_back(Random.RandInt() % (PIPEGAPMAX + 1), PIPEXMAX + SCREENWIDTH / 2);
	//pipes.emplace_back(Random.RandInt() % (PIPEGAPMAX+1), PIPEXMAX+SCREENWIDTH);
	// Three pipes proved to be a bit too much.

	//Pipe pipe(Random.RandInt() % (PIPEGAPMAX+1), PIPEXMAX);
	Bird bird(BIRDYSTART, score);

	int waitingforup = 0; // Workaround to make the touch code edge-triggered rather than level-triggered

	while (!bird.is_dead()) {
		// pretty colors
		LCD.Clear();
		backdrop.render();
		for (Pipe &pipe : pipes)
			pipe.render();
		bird.render();
		score.render();
		LCD.Update();

		// mutable updates
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
			bird.feedCollision(pipe); // They're pretty yummy
		}

		// reap for a dead pipe and throw a new one at the back
		for (auto it = pipes.begin(); it != pipes.end(); ++it) {
			if (it->is_dead()) {
				pipes.erase(it);
				pipes.emplace_back(Random.RandInt() % (PIPEGAPMAX + 1), PIPEXMAX + PIPEWIDTH);
				break;
			}
		}
		//Sleep(20);
	}
	// If you're reading this, it's too late.

	FEHFile *leaderboard;

	leaderboard = SD.FOpen("High Scores.txt", "a"); // Open file in "append" mode.
	SD.FPrintf(leaderboard, "%i\n", score.score()); // Blindly throw the new score on a line at the end
	SD.FClose(leaderboard);

	LCD.SetBackgroundColor(BLACK);
	LCD.Clear();

	LCD.SetFontColor(0xFFFFFF);
	LCD.WriteRC("Loading...", 0, 0);
	LCD.Update();

	display_image("img/bob.txt");

	LCD.SetFontColor(0);
	LCD.FillRectangle(12, 33, 90, 22);
	LCD.FillRectangle(12, 56, 174, 22);

	LCD.SetFontColor(0xFFFFFF);
	LCD.DrawRectangle(12, 33, 90, 22);
	LCD.DrawRectangle(12, 56, 174, 22);

	LCD.SetFontColor(0xFF0000);
	LCD.WriteRC("You died! Score:", 0, 0);
	LCD.WriteRC(score.score(), 0, 17);

	LCD.SetFontColor(0xFFFFFF);
	LCD.WriteAt("Restart", 14, 37);
	LCD.WriteAt("Return to menu", 14, 60);

	bool selected = false;
	enum NextState next_state;

	while (!selected) {
		while (!LCD.Touch(&touchx, &touchy))
			;
		if (touchx >= 12 && touchx < 12 + 90 && touchy >= 33 && touchy < 33 + 22) {
			selected = true;
			next_state = PLAY_GAME;
		} else if (touchx >= 12 && touchx < 12 + 174 && touchy >= 56 && touchy < 56 + 22) {
			selected = true;
			next_state = MAIN_MENU;
		}
	}

	return next_state;
}


// Show quit screen
//Robbie
void do_quit() {
	// When I tried to make the quit button actually quit the game, somehow it flipped a coin to see
	// if the game would load normally or quit on its own before you could even hit "play". I didn't want to
	// get rid of the button, so I just made it softlock with an unsettling image to encourage the player to
	// close the game manually.

	LCD.SetBackgroundColor(0);
	LCD.Clear();

	LCD.SetFontColor(0xFFFFFF);
	LCD.WriteRC("Loading...", 0, 0);
	LCD.Update();

	display_image("img/unsure.txt");
	LCD.SetFontColor(BLACK); // The next 4 lines emulate a text border that wraps around characters rather than a region.
	LCD.WriteAt("There is no escape.", 45, 111);
	LCD.WriteAt("There is no escape.", 47, 113);
	LCD.WriteAt("There is no escape.", 45, 113);
	LCD.WriteAt("There is no escape.", 47, 111);
	LCD.SetFontColor(RED);
	LCD.WriteAt("There is no escape.", 46, 112);

	while (1) LCD.Update();
}

/*
 * Entry point to the application
 */
int main() {
	// This game is implemented as a half-hearted state machine, where each screen returns the
	// next screen as a "next state", which main() processes accordingly.
	enum NextState next_state = MAIN_MENU;
	bool quit = false;
	while (!quit) {
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
		case QUIT:
			// See do_quit() for why this doesn't actually quit the game.
			//quit = true;
			do_quit();
			break;
		}
	}
	return 0;
}
