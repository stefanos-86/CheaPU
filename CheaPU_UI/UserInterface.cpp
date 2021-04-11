#include "UserInterface.h"

#include <stdexcept>

#include <sstream>
#include <iostream>

namespace CheaPU {

	static void sdl_null_check(const void* pointer) {
		if (!pointer)
			throw std::runtime_error(SDL_GetError());
	}

	static void sdl_return_check(const int rc) {
		if (rc != 0)
			throw std::runtime_error(SDL_GetError());
	}

	UserInterface::UserInterface() :
		main_window(nullptr),
		main_window_surface(nullptr),
		renderer(nullptr),
		halt_game_loop(true)
	{
		// Define all the widgets.
		reset_button = button_area(25, 200);
		halt_button = button_area(70, 200);
		enter_button = button_area(285, 80);
		tape_button = button_area(355, 80);

		uint8_t button_step = 0;
		for (ToggleButton& t : address_buttons) {
			t.up = false;
			t.area = button_area(180, 15 + button_step);
			button_step += 30;
		}

		button_step = 0;
		for (ToggleButton& t : value_buttons) {
			t.up = false;
			t.area = button_area(240, 15 + button_step);
			button_step += 30;
		}

		// The holes in the paper tapes are buttons. You can punch (or "glue shut") the holes
		// with a click. The matrix corresponds to whatever goes in memory, directly - even if, as far as I know
		// real life paper tapes had 5 columns (for Baudot's code insteda of ASCII letters).
		uint8_t hole_horizontal_margin = 4;
		uint8_t hole_vertical_margin = 6;
		uint16_t col = 400 + hole_horizontal_margin;
		uint16_t row = hole_vertical_margin;
		for (auto& line_of_holes : tape_holes) {
			for (ToggleButton& t : line_of_holes) {
				t.area.x = col;
				t.area.y = row;
				t.area.h = 15;
				t.area.w = 15;
				col += hole_horizontal_margin + 15;
			}
			col = 400 + hole_horizontal_margin;
			row += hole_vertical_margin + 15;
		}
	}

	UserInterface::~UserInterface()
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(main_window);
		SDL_Quit();
	}

	void UserInterface::open_window()
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
			throw std::runtime_error(SDL_GetError());

		main_window = SDL_CreateWindow(
			"Go download a real emulator...",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			UserInterface::SCREEN_WIDTH, UserInterface::SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);
		sdl_null_check(main_window);

		main_window_surface = SDL_GetWindowSurface(main_window);
		sdl_null_check(main_window_surface);

		renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		sdl_null_check(renderer);
	}

	void UserInterface::poll_input()
	{
		SDL_Event user_input;
		while (SDL_PollEvent(&user_input) != 0)

			// Quit commands have priority over everything. Bail out and ask no questions.
			if (user_input.type == SDL_QUIT) {
				halt_game_loop = true;
				return;
			}
			else if (user_input.type == SDL_KEYDOWN  && user_input.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
				halt_game_loop = true;
				return;
			}

			else if (user_input.type == SDL_MOUSEBUTTONDOWN) {
				int mouseX = user_input.motion.x;
				int mouseY = user_input.motion.y;
#ifdef NOT_USED
				/* Debug code - keep it handy in case of future work.
				   Thanks to  https://gigi.nullneuron.net/gigilabs/handling-keyboard-and-mouse-events-in-sdl2/ */
				std::stringstream ss;
				ss << "";
				ss << "X: " << mouseX << " Y: " << mouseY;
#endif

				SDL_Point click_location;
				click_location.x = mouseX;
				click_location.y = mouseY;

				switch (user_input.button.button)
				{
				case SDL_BUTTON_LEFT:
					if (SDL_PointInRect(&click_location, &reset_button))
						cpu.reset();

					if (SDL_PointInRect(&click_location, &halt_button))
						cpu.error = true;
					
					if (SDL_PointInRect(&click_location, &enter_button)) {
						const uint8_t address = read_byte(address_buttons);
						const uint8_t value = read_byte(value_buttons);
						memory[address] = value;

						// Debug code - may be very useful. 
						// std::cout << "Address " << (int) address << " value " << (int) value << std::endl;
					}

					if (SDL_PointInRect(&click_location, &tape_button)) {
						uint8_t address = 0;
						for (const auto& line_of_holes : tape_holes) {
							const uint8_t value = read_byte(line_of_holes);
							memory[address] = value;
							// Debug code - may be very useful. 
							// std::cout << "Address " << (int) address << " value " << (int) value << std::endl;
							++address;
						}
					}

					for (ToggleButton& t : address_buttons)
						if (SDL_PointInRect(&click_location, &t.area))
							t.up = ! t.up;

					for (ToggleButton& t : value_buttons)
						if (SDL_PointInRect(&click_location, &t.area))
							t.up = !t.up;


					for (auto& line_of_holes : tape_holes)
						for (ToggleButton& t : line_of_holes)
							if (SDL_PointInRect(&click_location, &t.area))
								t.up = !t.up;
					break;
#ifdef NOT_USED
				case SDL_BUTTON_RIGHT:
					SDL_ShowSimpleMessageBox(0, "Debug", ss.str().c_str(), main_window);
					break;
#endif
				default:
					// Use only left button, ignore everything else.
					break;
				}
			}
	}

	void UserInterface::draw_background() {
		int rc = 0;

		rc = SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
		sdl_return_check(rc);

		rc = SDL_RenderClear(renderer);
		sdl_return_check(rc);
	}

	void UserInterface::draw_led(const uint16_t top, const uint16_t left, const bool onOrOff)
	{
		static constexpr uint16_t led_size_px = 25;

		if (onOrOff)
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		else
			SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);

		SDL_Rect lamp;
		lamp.x = left;
		lamp.y = top;
		lamp.h = led_size_px;
		lamp.w = led_size_px;

		SDL_RenderFillRect(renderer, &lamp);
	}


	void UserInterface::draw_button(SDL_Rect button_square, const bool up)
	{
		SDL_Rect button_proper;
		button_proper.x = button_square.x + UserInterface::border_size;
		button_proper.y = button_square.y + UserInterface::border_size;
		button_proper.h = button_square.h - UserInterface::border_size;
		button_proper.w = button_square.w - UserInterface::border_size;


		SDL_SetRenderDrawColor(renderer, 90, 90, 90, 255);
		SDL_RenderFillRect(renderer, &button_square);

		if (up)
			SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
		else
			SDL_SetRenderDrawColor(renderer, 110, 110, 110, 255);

		SDL_RenderFillRect(renderer, &button_proper);
	}

	void UserInterface::draw_text(const uint16_t top, const uint16_t left, const uint16_t size_px, char c)
	{
		SDL_Surface* surface;
		
		// This comes out of the docs... I don't expect it to ever be needed, but...
		Uint32 rmask, gmask, bmask, amask;
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				rmask = 0xff000000;
				gmask = 0x00ff0000;
				bmask = 0x0000ff00;
				amask = 0x000000ff;
		#else
				rmask = 0x000000ff;
				gmask = 0x0000ff00;
				bmask = 0x00ff0000;
				amask = 0xff000000;
		#endif

		surface = SDL_CreateRGBSurface(0, 8, 8, 32, rmask, gmask, bmask, amask);
		sdl_null_check(surface);

		SDL_Rect entire_surface;
		entire_surface.x = 0;
		entire_surface.y = 0;
		entire_surface.h = 8;
		entire_surface.w = 8;

		const Uint32 color = SDL_MapRGB(surface->format, 255, 255, 255);
		const Uint32 background = SDL_MapRGB(surface->format, 150, 150, 150);

		if (c < 'A' || c > 'Z')
			c = '@'; // Which is just before 'A';

		SDL_LockSurface(surface);

		SDL_FillRect(surface, &entire_surface, background);
		
		Uint32* cursor = (Uint32*)surface->pixels;
		for (uint8_t byte = 0; byte < 8; ++byte) {
			uint8_t line_byte = petscii[(c - '@') * 8 + byte];
			for (uint8_t bit = 0; bit < 8; ++bit) {
				if (0x80 & line_byte)
					SDL_memset(cursor, color, sizeof(color));
				line_byte = line_byte << 1;
				++cursor;
			}
		}

		SDL_UnlockSurface(surface);

		SDL_Rect to;
		to.x = left;
		to.y = top;
		to.h = size_px;
		to.w = size_px;

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		const int rc = SDL_RenderCopy(renderer, texture, &entire_surface, &to);
		sdl_return_check(rc);

		SDL_DestroyTexture(texture);
		SDL_FreeSurface(surface);
	}

	void UserInterface::draw_text(const uint16_t top, const uint16_t left, const uint16_t size_px, const std::string& text)
	{
		uint16_t cursor = left;
		for (const char c : text) {
			draw_text(top, cursor, size_px, c);
			cursor += size_px;
		}
	}

	void UserInterface::draw_tape()
	{
		SDL_Rect paper;
		paper.x = 400;
		paper.y = 0;
		paper.w = 156;
		paper.h = UserInterface::SCREEN_HEIGHT;

		SDL_SetRenderDrawColor(renderer, 250, 250, 250, 255);
		int rc = SDL_RenderFillRect(renderer, &paper);
		sdl_return_check(rc);

		for (const auto& row : tape_holes) {
			for (const ToggleButton& t : row)
			{
				if (t.up)
					SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);  // Ones.
				else
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);        // Zeroes.

				int rc = SDL_RenderFillRect(renderer, &t.area);
				sdl_return_check(rc);
			}
		}
	}

	SDL_Rect UserInterface::button_area(const uint16_t top, const uint16_t left) const
	{
		SDL_Rect area;
		area.x = left;
		area.y = top;
		area.w = UserInterface::button_size;
		area.h = UserInterface::button_size;

		return area;
	}

	uint8_t UserInterface::read_byte(const std::array<ToggleButton, 8>& buttons) const
	{
		uint8_t byte = 0;
		uint8_t mask = 0x80;  // Read left to right.
		for (const ToggleButton& t : buttons) {
			if (t.up)
				byte = byte | mask;
			mask = mask >> 1;
		}

		return byte;
	}


	void UserInterface::game_loop()
	{
		cpu.reset();
		memory[0x00] = 86;

		halt_game_loop = false;
		while (!halt_game_loop) {
			poll_input();

			if (halt_game_loop)
				return;

			cpu.cycle(memory);  // 1 cicle per frame... not the fastest thing around, but makes the LED blinks at a nice pace.

			draw_background();

			draw_text(10, 10, 20, "COMPUTER");

			draw_text(40, 15, 10, "OVER");
			draw_text(40, 65, 10, "ZERO");
			draw_text(40, 115, 10, "ERROR");
			
			draw_led(55, 15, cpu.overflow);
			draw_led(55, 65, cpu.zero);
			draw_led(55, 115, cpu.error);

			draw_text(100, 15, 10, "ACCUMULATOR");
			draw_led(120, 15, (cpu.accumulator & 0x80));
			draw_led(120, 45, (cpu.accumulator & 0x40));
			draw_led(120, 75, (cpu.accumulator & 0x20));
			draw_led(120, 105, (cpu.accumulator & 0x10));
			draw_led(120, 135, (cpu.accumulator & 0x08));
			draw_led(120, 165, (cpu.accumulator & 0x04));
			draw_led(120, 195, (cpu.accumulator & 0x02));
			draw_led(120, 225, (cpu.accumulator & 0x01));

			draw_text(10, 200, 10, "RESET");
			draw_button(reset_button, true);

			draw_text(55, 200, 10, "HALT");
			draw_button(halt_button, true);

			draw_text(165, 15, 10, "ADDRESS");
			for (const ToggleButton& t : address_buttons)
				draw_button(t.area, t.up);

			draw_text(225, 15, 10, "VALUE");
			for (const ToggleButton& t : value_buttons)
				draw_button(t.area, t.up);

			draw_text(285, 15, 10, "ENTER");
			draw_button(enter_button, true);

			draw_text(335, 15, 10, "LOADTAPE");
			draw_button(tape_button, true);

			draw_tape();

			SDL_RenderPresent(renderer);
		}

	}

}
