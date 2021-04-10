/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Palette.h"
#include "../Engine/Screen.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Surface.h"
#include "../Engine/Exception.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Interface/NumberText.h"
#include "../Interface/Slider.h"
#include "../Interface/ComboBox.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the test screen.
 * @param game Pointer to the core game.
 */
TestState::TestState()
{
	// Create objects
	_window = new Window(this, 300, 180, 10, 10);
	_text = new Text(280, 120, 20, 50);
	_button = new TextButton(100, 20, 110, 150);
	_list = new TextList(300, 180, 10, 10);
	_number = new NumberText(50, 5, 200, 25);
	_set = _game->getMod()->getSurfaceSet("BASEBITS.PCK");
	_set->getFrame(1);
	_slider = new Slider(100, 15, 50, 50);
	_comboBox = new ComboBox(this, 80, 16, 98, 100);
	// Set palette
	setPalette("PAL_BASESCAPE", 2);

	add(_window);
	add(_button);
	add(_text);
	add(_list);
	add(_number);
	add(_slider);
	add(_comboBox);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+1);
	_window->setBackground(_game->getMod()->getSurface("BACK04.SCR"));

	_button->setColor(Palette::blockOffset(15)+1);
	_button->setText("LOLOLOL");

	_text->setColor(Palette::blockOffset(15)+1);
	//_text->setBig();
	_text->setWordWrap(true);
	_text->setAlign(ALIGN_CENTER);
	_text->setVerticalAlign(ALIGN_MIDDLE);
	//_text->setText(tr("STR_COUNCIL_TERMINATED"));

	_list->setColor(Palette::blockOffset(15)+1);
	_list->setColumns(3, 100, 50, 100);
	_list->addRow(2, "a", "b");
	_list->addRow(3, "lol", "welp", "yo");
	_list->addRow(1, "0123456789");

	_number->setColor(Palette::blockOffset(15) + 1);
	_number->setValue(1234567890);

	_slider->setColor(Palette::blockOffset(15)+1);

	std::vector<std::string> difficulty;
	for (int i = 0; i != 3; ++i)
	{
		difficulty.push_back(tr("STR_1_BEGINNER"));
		difficulty.push_back(tr("STR_2_EXPERIENCED"));
		difficulty.push_back(tr("STR_3_VETERAN"));
		difficulty.push_back(tr("STR_4_GENIUS"));
		difficulty.push_back(tr("STR_5_SUPERHUMAN"));
	}

	_comboBox->setColor(Palette::blockOffset(15)+1);
	_comboBox->setOptions(difficulty);

	_i = 0;

	//_game->getMod()->getPalette("PAL_GEOSCAPE")->savePal("../../../Geoscape.pal");
	//_game->getMod()->getPalette("PAL_BASESCAPE")->savePal("../../../Basescape.pal");
	//_game->getMod()->getPalette("PAL_UFOPAEDIA")->savePal("../../../Ufopaedia.pal");
	//_game->getMod()->getPalette("PAL_BATTLESCAPE")->savePal("../../../Battlescape.pal");

	//_game->getMod()->getFont("FONT_BIG")->fix("../../../Big.bmp", 256);
	//_game->getMod()->getFont("FONT_SMALL")->fix("../../../Small.bmp", 128);
}

TestState::~TestState()
{

}

void TestState::think()
{
	State::think();

	/*
	_text->setText(tr(_i));
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
	surface = SDL_CreateRGBSurface(0, 256, 25, 8, 0, 0, 0, 0);

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
