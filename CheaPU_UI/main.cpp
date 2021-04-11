#include "UserInterface.h"


int main(void) {
	CheaPU::UserInterface ui;
	ui.open_window();
	ui.game_loop();
	return 0;
}