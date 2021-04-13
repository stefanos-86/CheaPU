#pragma once

#include <string>
#include <array>

#include "CPU.h"
#include "MemoryChip.h"

#include <SDL.h>
#undef main  // https://stackoverflow.com/questions/6847360/error-lnk2019-unresolved-external-symbol-main-referenced-in-function-tmainc


namespace CheaPU {

	/** Helper to connect the button value and the rect it is displayied in - it makes it easy
	    to pass the rect back to SDL when checking the mouse click hits. */
	struct ToggleButton {
		bool up;
		SDL_Rect area;
	};

	/** "Boilerplate" to hold the SDL interface, paint the UI and react to clicks.
	
	Most of it is a copy-paste from another project I had already done. There is no other reason
	for the use of SDL or the structure of this code other than "this is already done and works well enough".
	
	The interface is "immediate" (redraw everything at every loop), because it is easy to do.
	The layout is entirely based on magic numbers scattered everywhere because I do not care much - I don't
	plan to maintain this code in the long run.

	Check also the cpp file's comments for more details about the specific parts.
	*/
	class UserInterface
	{
	public:
		static constexpr int SCREEN_WIDTH = 640;
		static constexpr int SCREEN_HEIGHT = 480;

		/** Try not to create more than one! It instantiates SDL structures on creation.*/
		UserInterface();
		~UserInterface();
		
		void open_window();
		void game_loop();

	private:
		SDL_Window* main_window;
		SDL_Surface* main_window_surface;
		SDL_Renderer* renderer;

		bool halt_game_loop;

		UserInterface(const UserInterface&) = delete;
		void operator=(const UserInterface&) = delete;

		void poll_input();
		void draw_background();
		void draw_led(const uint16_t top, const uint16_t left, const bool onOrOff);
		void draw_button(SDL_Rect button_square, const bool up);
		void draw_text(const uint16_t top, const uint16_t left, const uint16_t size_px, char c);
		void draw_text(const uint16_t top, const uint16_t left, const uint16_t size_px, const std::string& text);
		void draw_tape();

		/**Construct the rect for the button. The size is standard.*/
		SDL_Rect button_area(const uint16_t top, const uint16_t left) const;

		/** Takes the 8 buttons and pick a bit from every one. Buttons are supposed to be left-to-right on
		    the display. It puts the 1st bit in the most significant position (the machine is big endian). */
		uint8_t read_byte(const std::array<ToggleButton, 8>& buttons) const;

		static constexpr uint16_t button_size = 25;
		static constexpr uint16_t border_size = 3;

		SDL_Rect reset_button;
		SDL_Rect enter_button;
		SDL_Rect halt_button;
		SDL_Rect tape_button;
		std::array<ToggleButton, 8> address_buttons;
		std::array<ToggleButton, 8> value_buttons;

		std::array< std::array<ToggleButton, 8>, 22> tape_holes;

		CPU cpu;
		MemoryChip memory;

		/** Since we are emulating a primitive microcomputer, I feel I should implement the text
		* rendering how it was done "back then".
		* 
		* Well, no. The real reason is that I don't want to reuse some code I have that expects an image with the 
		* character maps nor link in the appropriate SDL extension. This is just a reasonably simple kludge
		* (with a pedigree).
		* 
		* A letter is an 8x8 matrix of pixel. Using one bit each, you can have a glyph defined by 8 bytes.
		* Then, it is a  matter of looping trough the bytes and bit and drawing each dot at the right place,
		* but an expert implementation would just SDL_memset the bytes from this array into the SDL surfaces.
		* 
		* To keep in tune with the "nostalgic flavour" I got the bits that define the Commodore64 font from http://petscii.krissz.hu/*/
		static constexpr uint8_t petscii[] = {
			60,102,110,110,96,98,60,0,  // @, used as a "anything I don't have" value, because it's just before 'A' - makes the ASCII-(this array) conversion easy.
			24,60,102,126,102,102,102,0,  // A
			124,102,102,124,102,102,124,0, // B
			60,102,96,96,96,102,60,0, //...
			120,108,102,102,102,108,120,0,
			126,96,96,120,96,96,126,0,
			126,96,96,120,96,96,96,0,
			60,102,96,110,102,102,60,0,
			102,102,102,126,102,102,102,0,
			60,24,24,24,24,24,60,0,
			30,12,12,12,12,108,56,0,
			30,12,12,12,12,108,56,0,
			96,96,96,96,96,96,126,0,
			99,119,127,107,99,99,99,0,
			102,118,126,126,110,102,102,0,
			60,102,102,102,102,102,60,0,
			124,102,102,124,96,96,96,0,
			60,102,102,102,102,60,14,0,
			124,102,102,124,120,108,102,0,
			60,102,96,60,6,102,60,0,
			126,24,24,24,24,24,24,0,
			102,102,102,102,102,102,60,0,
			102,102,102,102,102,60,24,0,
			99,99,99,107,127,119,99,0,
			102,102,60,24,60,102,102,0,
			102,102,102,60,24,24,24,0,
			126,6,12,24,48,96,126,0  // ...Z
		};
	};

}
