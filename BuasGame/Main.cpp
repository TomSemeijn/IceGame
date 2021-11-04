#if true
	#define CATCHEXEPTIONS
#endif

#include "IceGame.h"
#include <SFML/Graphics.hpp>

const bool MEMORYPROFILE = false;
const std::string MAINFONTDIR = "Content/Fonts/Prototype.ttf";
sf::Font mainFont;

//main function for game without console window (I'm not sure why but it doesn't work with the normal main function)
int WinMain() {

#ifdef CATCHEXEPTIONS
	try {
#endif
		//tries to load the main font
		if (!mainFont.loadFromFile(MAINFONTDIR)) {
			return 1;
		}

		//gives a chance to profile the heap before the game
		if (MEMORYPROFILE) {
			char fin;
			do {
				std::cin >> fin;
			} while (fin != 's');
		}

		{
			IceGame game{ };
			game.startGame();
		}

		//gives a chance to profile the heap after the game
		if (MEMORYPROFILE) {
			char in;
			do {
				std::cin >> in;
			} while (in != 'q');
		}
#ifdef CATCHEXEPTIONS
	}
	catch (std::exception& exc) {
		writeLn("aborted game because ", exc.what());
	}
#endif

	return 0;
}

//normal main function
int main() {
	return WinMain();
}
