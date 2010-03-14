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
#include "Game.h"
#include "State_Interactive.h"

Game::Game(char* title, int width, int height, int bpp) : _state(NULL), _newState(NULL), _palettes(), _fonts(), _languages(), _surfaces(), _sets(), _quit(false)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		throw SDL_GetError();
	}

	// Set the window caption
	SDL_WM_SetCaption(title, NULL);

	// Create display
	_screen = new Screen(width, height, bpp);

	// Create cursor
	_cursor = new Cursor(9, 13);
	_cursor->setColor(Palette::blockOffset(15)+12);
}

Game::~Game()
{
	delete _screen;
	delete _state;
	
	for (map<string, Language*>::iterator i = _languages.begin(); i != _languages.end(); i++) {
		delete i->second;
	}
	for (map<string, Font*>::iterator i = _fonts.begin(); i != _fonts.end(); i++) {
		delete i->second;
	}
	for (map<string, Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); i++) {
		delete i->second;
	}
	for (map<string, SurfaceSet*>::iterator i = _sets.begin(); i != _sets.end(); i++) {
		delete i->second;
	}
	for (map<string, Palette*>::iterator i = _palettes.begin(); i != _palettes.end(); i++) {
		delete i->second;
	}
}

void Game::load()
{
	// Load palettes
	for (int i = 0; i < 5; i++)
	{
		stringstream s1, s2;
		s1 << DATA_FOLDER << "GEODATA/PALETTES.DAT";
		s2 << "PALETTES.DAT_" << i;
		_palettes[s2.str()] = new Palette();
		_palettes[s2.str()]->loadDat(s1.str(), 256, Palette::palOffset(i));
		cout << s2.str() << endl;
	}

	stringstream s1, s2;
	s1 << DATA_FOLDER << "GEODATA/BACKPALS.DAT";
	s2 << "BACKPALS.DAT";
	_palettes[s2.str()] = new Palette();
	_palettes[s2.str()]->loadDat(s1.str(), 128);
	cout << s2.str() << endl;
	
	// Load fonts
	string font[] = {"BIGLETS.DAT",
					 "SMALLSET.DAT"};
	
	for (int i = 0; i < 2; i++)
	{
		stringstream s;
		s << DATA_FOLDER << "GEODATA/" << font[i];
		if (font[i] == "BIGLETS.DAT")
			_fonts[font[i]] = new Font(16, 16, 173);
		else if (font[i] == "SMALLSET.DAT")
			_fonts[font[i]] = new Font(8, 9, 173, -1);
		_fonts[font[i]]->getSurface()->loadScr(s.str());
		_fonts[font[i]]->load();
		cout << font[i] << endl;
	}

	// Load languages
	string lang[] = {"ENGLISH.DAT",
					 "FRENCH.DAT",
					 "GERMAN.DAT"};

	for (int i = 0; i < 3; i++)
	{
		stringstream s;
		s << DATA_FOLDER << "GEODATA/" << lang[i];
		_languages[lang[i]] = new Language();
		_languages[lang[i]]->loadDat(s.str());
		cout << lang[i] << endl;
	}

	_currentLang = _languages["ENGLISH.DAT"];
	_langName = "ENGLISH.DAT";

	// Load surfaces
	string dats[] = {"LANG1.DAT",
					 "LANG2.DAT"};

	for (int i = 0; i < 2; i++)
	{
		stringstream s;
		s << DATA_FOLDER << "GEODATA/" << dats[i];
		_surfaces[dats[i]] = new Surface(64, 154);
		_surfaces[dats[i]]->loadScr(s.str());
		cout << dats[i] << endl;
	}

	string scrs[] = {"BACK01.SCR",
					 "BACK02.SCR",
					 "BACK03.SCR",
					 "BACK04.SCR",
					 "BACK05.SCR",
					 "BACK06.SCR",
					 "BACK07.SCR",
					 "BACK08.SCR",
					 "BACK09.SCR",
					 "BACK10.SCR",
					 "BACK11.SCR",
					 "BACK12.SCR",
					 "BACK13.SCR",
					 "BACK14.SCR",
					 "BACK15.SCR",
					 "BACK16.SCR",
					 "BACK17.SCR",
					 "GEOBORD.SCR",
					 "UP_BORD2.SCR"};

	for (int i = 0; i < 19; i++)
	{
		stringstream s;
		s << DATA_FOLDER << "GEOGRAPH/" << scrs[i];
		_surfaces[scrs[i]] = new Surface(320, 200);
		_surfaces[scrs[i]]->loadScr(s.str());
		cout << scrs[i] << endl;
	}

	string spks[] = {"UP001.SPK",
					 "UP002.SPK",
					 "UP003.SPK",
					 "UP004.SPK",
					 "UP005.SPK",
					 "UP006.SPK",
					 "UP007.SPK",
					 "UP008.SPK",
					 "UP009.SPK",
					 "UP010.SPK",
					 "UP011.SPK",
					 "UP012.SPK",
					 "UP013.SPK",
					 "UP014.SPK",
					 "UP015.SPK",
					 "UP016.SPK",
					 "UP017.SPK",
					 "UP018.SPK",
					 "UP019.SPK",
					 "UP020.SPK",
					 "UP021.SPK",
					 "UP022.SPK",
					 "UP023.SPK",
					 "UP024.SPK",
					 "UP025.SPK",
					 "UP026.SPK",
					 "UP027.SPK",
					 "UP028.SPK",
					 "UP029.SPK",
					 "UP030.SPK",
					 "UP031.SPK",
					 "UP032.SPK",
					 "UP033.SPK",
					 "UP034.SPK",
					 "UP035.SPK",
					 "UP036.SPK",
					 "UP037.SPK",
					 "UP038.SPK",
					 "UP039.SPK",
					 "UP040.SPK",
					 "UP041.SPK",
					 "UP042.SPK",
					 "GRAPHS.SPK"};

	for (int i = 0; i < 43; i++)
	{
		stringstream s;
		s << DATA_FOLDER << "GEOGRAPH/" << spks[i];
		_surfaces[spks[i]] = new Surface(320, 200);
		_surfaces[spks[i]]->loadSpk(s.str());
		cout << spks[i] << endl;
	}

	// Load surface sets
	string pcks[] = {"BASEBITS.PCK",
					 "INTICON.PCK",
					 "TEXTURE.DAT"};

	for (int i = 0; i < 3; i++)
	{
		stringstream s;
		s << DATA_FOLDER << "GEOGRAPH/" << pcks[i];
		string ext = pcks[i].substr(pcks[i].length()-3, pcks[i].length());
		if (ext == "PCK")
		{
			_sets[pcks[i]] = new SurfaceSet(32, 40);
			_sets[pcks[i]]->loadPck(s.str());
		}
		else
		{
			_sets[pcks[i]] = new SurfaceSet(32, 32);
			_sets[pcks[i]]->loadDat(s.str());
		}
		cout << pcks[i] << endl;
	}
}

void Game::run()
{
	while (!_quit)
	{
		// Process state change
		if (_newState != NULL)
		{
			if (_state != NULL)
			{
				delete _state;
			}
			_state = _newState;
			_newState = NULL;
		}

		// Process events
		while (SDL_PollEvent(&_event))
		{
			if (_event.type == SDL_QUIT)
			{
				_quit = true;
			}
			else
			{
				_cursor->handle(&_event, _screen->getScale());
				_state->handle(&_event, _screen->getScale());
			}
		}
		
		// Process logic
		_state->think();
		
		// Process rendering
		_screen->clear();
		_state->blit();
		_cursor->blit(_screen->getSurface());
		_screen->flip();
	}
}

void Game::quit()
{
	_quit = true;
}

Screen *Game::getScreen()
{
	return _screen;
}

Language *Game::getLanguage()
{
	return _currentLang;
}

string Game::getLanguageName()
{
	return _langName;
}

void Game::setLanguage(string lang)
{
	_currentLang = _languages[lang];
	_langName = lang;
}

Font *Game::getFont(string name)
{
	return _fonts[name];
}

Surface *Game::getSurface(string name)
{
	return _surfaces[name];
}

SurfaceSet *Game::getSurfaceSet(string name)
{
	return _sets[name];
}

Palette *Game::getPalette(string name)
{
	return _palettes[name];
}

void Game::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	_screen->setPalette(colors, firstcolor, ncolors);
	_cursor->setPalette(colors, firstcolor, ncolors);
	for (map<string, Font*>::iterator i = _fonts.begin(); i != _fonts.end(); i++) {
		i->second->getSurface()->setPalette(colors, firstcolor, ncolors);
	}
	for (map<string, Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); i++) {
		i->second->setPalette(colors, firstcolor, ncolors);
	}
	for (map<string, SurfaceSet*>::iterator i = _sets.begin(); i != _sets.end(); i++) {
		i->second->getSurface()->setPalette(colors, firstcolor, ncolors);
	}
}

void Game::setState(State *state)
{
	_newState = state;
}
