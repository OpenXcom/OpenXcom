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
#include "XcomResourcePack.h"

/**
 * Initializes the resource pack by loading all the resources
 * contained in the original game folder.
 * @param folder Subfolder to load resources from.
 */
XcomResourcePack::XcomResourcePack(string folder) : ResourcePack(folder)
{
	cout << "Loading resources..." << endl;

	// Load palettes
	for (int i = 0; i < 5; i++)
	{
		stringstream s1, s2;
		s1 << folder << "GEODATA/PALETTES.DAT";
		s2 << "PALETTES.DAT_" << i;
		_palettes[s2.str()] = new Palette();
		_palettes[s2.str()]->loadDat(s1.str(), 256, Palette::palOffset(i));
		cout << s2.str() << endl;
	}

	stringstream s1, s2;
	s1 << folder << "GEODATA/BACKPALS.DAT";
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
		s << folder << "GEODATA/" << font[i];
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
		s << folder << "GEODATA/" << lang[i];
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
		s << folder << "GEODATA/" << dats[i];
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
		s << folder << "GEOGRAPH/" << scrs[i];
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
		s << folder << "GEOGRAPH/" << spks[i];
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
		s << folder << "GEOGRAPH/" << pcks[i];
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

	// Load polygons
	stringstream s;
	s << folder << "GEODATA/" << "WORLD.DAT";
	Globe::loadDat(s.str(), &_polygons);
	cout << "WORLD.DAT" << endl;

	// Load musics
	string mus[] = {"GMSTORY",
					 "GMGEO1",
					 "GMGEO2"};
	string exts[] = {"OGG", "MP3", "MID"};

	for (int i = 0; i < 3; i++)
	{
		_musics[mus[i]] = new Music();
		for (int j = 0; j < 3; j++)
		{
			stringstream s;
			s << folder << "SOUND/" << mus[i] << "." << exts[j];
			if (_musics[mus[i]]->load(s.str()))
				break;
		}
		cout << mus[i] << endl;
	}
}

/**
 *
 */
XcomResourcePack::~XcomResourcePack()
{
}