/*
 * Copyright 2010-2013 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "TestState.h"
#include <iostream>
#include <fstream>
#include <SDL.h>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Screen.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Surface.h"
#include "../Engine/Exception.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{

void FontToBmp(const std::string &font, int w, int h)
{
	std::string dat = "./" + font + ".DAT";
	std::string bmp = "./" + font + ".BMP";
	Surface *s = new Surface(w, h*173);
	s->loadScr(dat);

	SDL_Color clr[8];
	clr[0].r = 0;
	clr[0].g = 0;
	clr[0].b = 0;
	for (int i = 1; i < 8; ++i)
	{
		clr[i].r = 256-i*32;
		clr[i].g = 256-i*32;
		clr[i].b = 256-i*32;
	}
	s->setPalette(clr, 0, 8);

	SDL_SaveBMP(s->getSurface(), bmp.c_str());
}

void BmpToFont(const std::string &font)
{
	std::string dat = "./" + font + ".DAT";
	std::string bmp = "./" + font + ".BMP";
	SDL_Surface *orig = SDL_LoadBMP(bmp.c_str());

	Surface *s = new Surface(orig->w, orig->h);

	SDL_Color clr[8];
	clr[0].r = 0;
	clr[0].g = 0;
	clr[0].b = 0;
	for (int i = 1; i < 8; ++i)
	{
		clr[i].r = 256-i*32;
		clr[i].g = 256-i*32;
		clr[i].b = 256-i*32;
	}
	s->setPalette(clr, 0, 8);

	SDL_BlitSurface(orig, 0, s->getSurface(), 0);

	std::ofstream out (dat.c_str(), std::ios::out | std::ios::binary);
	for (int y = 0; y < s->getHeight(); ++y)
	{
		for (int x = 0; x < s->getWidth(); ++x)
		{
			char c = s->getPixel(x, y);
			out.write(&c, 1);
		}
	}
	out.close();
}

/**
 * Initializes all the elements in the test screen.
 * @param game Pointer to the core game.
 */
TestState::TestState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 300, 180, 10, 10);
	_text = new Text(280, 120, 20, 50);
	_button = new TextButton(100, 20, 110, 150);
	_list = new TextList(300, 180, 10, 10);
	_set = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
	_set->getFrame(1);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_1")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);

	add(_window);
	add(_button);
	add(_text);
	add(_list);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK04.SCR"));

	_button->setColor(Palette::blockOffset(15)+1);
	_button->setText(L"LOLOLOL");

	_text->setColor(Palette::blockOffset(15)+1);
	//_text->setBig();
	_text->setWordWrap(true);
	_text->setAlign(ALIGN_CENTER);
	_text->setVerticalAlign(ALIGN_MIDDLE);
	//_text->setText(_game->getLanguage()->getString("STR_COUNCIL_TERMINATED"));

	_list->setColor(Palette::blockOffset(15)+1);
	_list->setColumns(3, 100, 50, 100);
	_list->addRow(2, L"a", L"b");
	_list->addRow(3, L"lol", L"welp", L"yo");
	_list->addRow(1, L"0123456789");

	_i = 0;

	//FontToBmp("../../fonts/BIGLETS_R", 16, 16);
	//FontToBmp("../../fonts/SMALLSET_R", 8, 9);
	//FontToBmp("../../fonts/BIGLETS_P", 16, 16);
	//FontToBmp("../../fonts/SMALLSET_P", 8, 9);
	BmpToFont("../../fonts/BIGLETS - New");
	BmpToFont("../../fonts/SMALLSET - New");
}

TestState::~TestState()
{

}

void TestState::think()
{
	State::think();

	/*
	_text->setText(_game->getLanguage()->getString(_i));
	_i++;
	*/
}

void TestState::blit()
{
	State::blit();

	_set->getFrame(0)->blit(_game->getScreen()->getSurface());
}

/**
 * Generates a surface with a row of every single color currently
 * loaded in the game palette (like a rainbow). First used for
 * testing 8bpp functionality, still useful for debugging palette issues.
 * @return Test surface.
 */
SDL_Surface *TestState::testSurface()
{
	SDL_Surface *surface;

	// Create surface
	surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 256, 25, 8, 0, 0, 0, 0);

	if (surface == 0)
	{
		throw Exception(SDL_GetError());
	}

	// Lock the surface
	SDL_LockSurface(surface);

	Uint8 *index = (Uint8 *)surface->pixels;

	for (int j = 0; j < 25; ++j)
		for (int i = 0; i < 256; i++, ++index)
			*index = i;

	// Unlock the surface
	SDL_UnlockSurface(surface);

	return surface;
}

}
