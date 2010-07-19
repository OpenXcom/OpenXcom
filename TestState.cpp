/*
 * Copyright 2010 Daniel Albano
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
#include "Game.h"
#include "ResourcePack.h"
#include "Language.h"
#include "LangString.h"
#include "Font.h"
#include "Palette.h"
#include "Screen.h"
#include "SurfaceSet.h"
#include "Surface.h"
#include "TextButton.h"
#include "Window.h"
#include "Text.h"
#include "TextList.h"

#include "SDL_mixer.h"
#include "SDL.h"
#include <fstream>

using namespace std;

/**
 * Initializes all the elements in the test screen.
 * @param game Pointer to the core game.
 */
TestState::TestState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(300, 180, 10, 10);
	_text = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 280, 160, 20, 50);
	_button = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 100, 20, 110, 150);
	_list = new TextList(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 300, 180, 10, 10);
	_set = game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
	_set->getFrame(1);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_1")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);

	add(_window);
	add(_button);
	add(_text);
	add(_list);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+4);
	_window->setBackground(game->getResourcePack()->getSurface("BACK04.SCR"));

	_button->setColor(Palette::blockOffset(15)+4);
	_button->setText("LOLOLOL");
	
	_text->setColor(Palette::blockOffset(15)+1);
	_text->setBig();
	_text->setWordWrap(true);
	_text->setAlign(ALIGN_CENTER);
	//_game->getResourcePack()->setLanguage("GERMAN.DAT");
	_text->setText(_game->getResourcePack()->getLanguage()->getString(STR_COUNCIL_TERMINATED));

	_list->setColor(Palette::blockOffset(15)+1);
	_list->setColumns(3, 100, 50, 100);
	_list->addRow(2, "a", "b");
	_list->addRow(3, "lol", "welp", "yo");
	_list->addRow(1, "0123456789");
	
	_i = 0;

	ifstream sndFile ("DATA/SOUND/SAMPLE.CAT", ios::in | ios::binary);
	if (!sndFile)
	{
		throw "Failed to load CAT";
	}

	int first;
	sndFile.read((char*)&first, sizeof(first));

	cout << first << endl;
	cout << first / sizeof(first) / 2 << endl;

	sndFile.seekg(0, ios::beg);

	int offset, size;
	sndFile.read((char*)&offset, sizeof(offset));
	sndFile.read((char*)&size, sizeof(size));

	cout << offset << endl;
	cout << size << endl;

	sndFile.seekg(offset, ios::beg);

	char namesize;
	char *name;

	sndFile.read(&namesize, 1);
	name = new char[namesize];
	sndFile.read(name, namesize);

	cout << name << endl;

	char *sound = new char[size];
	sndFile.read(sound, size);
	
	SDL_RWops *rw = SDL_RWFromMem(sound, size);

	Mix_Chunk *wav = Mix_LoadWAV_RW(rw, 1);
	if (wav == 0)
		cout << Mix_GetError() << endl;

	Mix_PlayChannel(-1, wav, 0);

	sndFile.close();
}

TestState::~TestState()
{
	
}

void TestState::think()
{
	State::think();

	/*
	_text->setText(_game->getResourcePack()->getLanguage()->getString(_i));
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
		throw SDL_GetError();
	}

	// Lock the surface
	SDL_LockSurface(surface);

	Uint8 *index = (Uint8 *)surface->pixels;
	
	for (int j = 0; j < 25; j++)
		for (int i = 0; i < 256; i++, index++)
			*index = i;

	// Unlock the surface
	SDL_UnlockSurface(surface);

	return surface;
}
