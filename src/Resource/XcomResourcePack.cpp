/*
 * Copyright 2010 OpenXcom Developers.
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
#include <sstream>
#include <sys/stat.h>
#include "../Engine/Palette.h"
#include "../Engine/Font.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Language.h"
#include "../Engine/Music.h"
#include "../Engine/GMCat.h"
#include "../Engine/SoundSet.h"
#include "../Geoscape/Globe.h"
#include "../Geoscape/Polygon.h"
#include "../Geoscape/Polyline.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "TerrainObjectSet.h"
#include "../Ruleset/MapBlock.h"
#include "../Ruleset/RuleTerrain.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/Node.h"
#include "../Savegame/NodeLink.h"
#include "../Battlescape/Position.h"

/**
 * Initializes the resource pack by loading all the resources
 * contained in the original game folder.
 * @param folder Subfolder to load resources from.
 */
XcomResourcePack::XcomResourcePack(const std::string &folder) : ResourcePack(folder)
{
	// Load palettes
	for (int i = 0; i < 5; i++)
	{
		std::stringstream s1, s2;
		s1 << folder << "GEODATA/PALETTES.DAT";
		s2 << "PALETTES.DAT_" << i;
		_palettes[s2.str()] = new Palette();
		_palettes[s2.str()]->loadDat(insensitive(s1.str()), 256, Palette::palOffset(i));
	}

	std::stringstream s1, s2;
	s1 << folder << "GEODATA/BACKPALS.DAT";
	s2 << "BACKPALS.DAT";
	_palettes[s2.str()] = new Palette();
	_palettes[s2.str()]->loadDat(insensitive(s1.str()), 128);
	
	// Load fonts
	std::string font[] = {"BIGLETS.DAT",
					 "SMALLSET.DAT"};
	
	for (int i = 0; i < 2; i++)
	{
		std::stringstream s;
		s << folder << "GEODATA/" << font[i];
		if (font[i] == "BIGLETS.DAT")
			_fonts[font[i]] = new Font(16, 16, 173);
		else if (font[i] == "SMALLSET.DAT")
			_fonts[font[i]] = new Font(8, 9, 173, -1);
		_fonts[font[i]]->getSurface()->loadScr(insensitive(s.str()));
		_fonts[font[i]]->load();
	}
		
	// Load languages
	std::string lang[] = {"ENGLISH.DAT",
					 "FRENCH.DAT",
					 "GERMAN.DAT"};

	for (int i = 0; i < 3; i++)
	{
		std::stringstream s;
		s << folder << "GEODATA/" << lang[i];
		_languages[lang[i]] = new Language();
		_languages[lang[i]]->loadDat(insensitive(s.str()));
	}

	_currentLang = _languages["ENGLISH.DAT"];
	_langName = "ENGLISH.DAT";

	// Load surfaces
	std::string dats[] = {"LANG1.DAT",
					 "LANG2.DAT"};

	for (int i = 0; i < 2; i++)
	{
		std::stringstream s;
		s << folder << "GEODATA/" << dats[i];
		_surfaces[dats[i]] = new Surface(64, 154);
		_surfaces[dats[i]]->loadScr(insensitive(s.str()));
	}

	{
		std::stringstream s;
		s << folder << "GEODATA/" << "INTERWIN.DAT";
		_surfaces["INTERWIN.DAT"] = new Surface(160, 556);
		_surfaces["INTERWIN.DAT"]->loadScr(insensitive(s.str()));
	}

	std::string scrs[] = {"BACK01.SCR",
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
		std::stringstream s;
		s << folder << "GEOGRAPH/" << scrs[i];
		_surfaces[scrs[i]] = new Surface(320, 200);
		_surfaces[scrs[i]]->loadScr(insensitive(s.str()));
	}

	std::string spks[] = {"UP001.SPK",
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
		std::stringstream s;
		s << folder << "GEOGRAPH/" << spks[i];
		_surfaces[spks[i]] = new Surface(320, 200);
		_surfaces[spks[i]]->loadSpk(insensitive(s.str()));
	}

	// Load surface sets
	std::string sets[] = {"BASEBITS.PCK",
					 "INTICON.PCK",
					 "TEXTURE.DAT"};

	for (int i = 0; i < 3; i++)
	{
		std::stringstream s;
		s << folder << "GEOGRAPH/" << sets[i];

		std::string ext = sets[i].substr(sets[i].length()-3, sets[i].length());
		if (ext == "PCK")
		{
			std::string tab = sets[i].substr(0, sets[i].length()-4) + ".TAB";
			std::stringstream s2;
			s2 << folder << "GEOGRAPH/" << tab;
			_sets[sets[i]] = new SurfaceSet(32, 40);
			_sets[sets[i]]->loadPck(insensitive(s.str()), insensitive(s2.str()));
		}
		else
		{
			_sets[sets[i]] = new SurfaceSet(32, 32);
			_sets[sets[i]]->loadDat(insensitive(s.str()));
		}
	}

	// Load polygons
	std::stringstream s;
	s << folder << "GEODATA/" << "WORLD.DAT";
	Globe::loadDat(insensitive(s.str()), &_polygons);

	// Load polylines (extracted from game)
	// -10 = Start of line
	// -20 = End of data
	double lines[] = {-10, 1.18901, -0.412334, 1.23918, -0.425424, 1.213, -0.471239, 1.22828, -0.490874, 1.23482, -0.482147, 1.30245, -0.541052, 1.29373,
					  -0.608684, 1.35918, -0.61741, 1.38099, -0.53887, 1.41154, -0.530144, 1.39626, -0.503964, 1.53153, -0.460331, 1.54025, -0.488692,
					  1.55116, -0.490874, 1.55334, -0.466876, 1.60352, -0.469057, 1.59916, -0.488692, 1.67552, -0.517054, 1.69515, -0.475602, 1.61661,
					  -0.386154, 1.61225, -0.436332, 1.56861, -0.440696, 1.56425, -0.460331, 1.54243, -0.462512, 1.53589, -0.449422, 1.55552, -0.373064,
					  -10, 6.13047, -0.726493, 6.17628, -0.726493, 6.1501, -0.645772, -10, 6.25264, -0.759218, 0.0109083, -0.73522, 0.0567232, -0.741765,
					  -10, 0.128718, -0.7614, 0.122173, -0.80067, 0.102538, -0.807215, 0.1309, -0.829031, 0.14399, -0.85303, 0.111265, -0.863938, 0.0719948,
					  -0.870483, 0.0501782, -0.885755, -10, 0.122173, -0.80067, 0.148353, -0.811578, 0.159261, -0.80067, 0.211621, -0.820305, 0.239983,
					  -0.811578, 0.239983, -0.794125, -10, 0.111265, -0.863938, 0.102538, -0.907571, 0.11781, -0.90539, 0.122173, -0.938114, -10, 0.139626,
					  -0.959931, 0.181078, -0.953386, -10, 0.248709, -0.942478, 0.261799, -0.887936, 0.213803, -0.877028, 0.242164, -0.85303, 0.229074,
					  -0.829031, 0.1309, -0.829031, -10, 0.0458149, -0.109083, 0.0479966, -0.148353, 0.0654498, -0.185441, 0.0698132, -0.237801, 0.0981748,
					  -0.244346, 0.122173, -0.224711, 0.17017, -0.222529, 0.231256, -0.235619, 0.257436, -0.211621, 0.19635, -0.113446, 0.176715, -0.126536,
					  0.148353, -0.0763582, -10, 0.438514, -0.554142, 0.436332, -0.383972, 0.595594, -0.383972, 0.628319, -0.410152, -10, 0.59123, -0.547597,
					  0.619592, -0.493056, -10, 0.283616, 0.4996, 0.349066, 0.495237, 0.349066, 0.434151, 0.362156, 0.469057, 0.407971, 0.440696, 0.447241,
					  0.449422, 0.510509, 0.386154, 0.545415, 0.390517, 0.558505, 0.469057, 0.575959, 0.464694, -10, 5.36252, 0.580322, 5.27962, 0.523599,
					  5.34071, 0.449422, 5.27089, 0.386154, 5.26653, 0.283616, 5.14436, 0.174533, 5.05491, 0.194168, 4.996, 0.14399, 5.01564, 0.0872665,
					  5.06364, 0.0763582, 5.06582, -0.0305433, 5.18145, -0.0370882, 5.15527, -0.0698132, 5.2229, -0.0938114, 5.2578, -0.019635, 5.35816,
					  -0.0327249, 5.38652, -0.0741765, -10, 4.10152, -0.85303, 4.45059, -0.85303, 4.62512, -0.855211, 4.71893, -0.837758, -10, 5.116, -0.776672,
					  5.08545, -0.824668, 5.03309, -0.785398, 4.97419, -0.785398, 4.95019, -0.770127, -10, 3.82227, -1.21519, 3.82227, -1.05374, 4.01426,
					  -0.977384, 3.95972, -0.949023, -10, 4.23897, -0.569414, 4.42659, -0.554142, 4.48113, -0.503964, 4.51386, -0.519235, 4.55531, -0.460331,
					  4.59022, -0.455967, -10, 4.82584, -0.728675, 4.84983, -0.750492, -10, 4.8062, -0.81376, 4.82802, -0.80067, -10, 0.545415, -1.21955, 0.549779,
					  -1.09738, 0.490874, -1.05156, -10, 0.488692, -1.04283, 0.490874, -0.981748, 0.569414, -0.933751, 0.554142, -0.909753, 0.698132, -0.863938,
					  0.665407, -0.818123, -10, 0.693768, -0.763582, 0.857393, -0.730857, -10, 0.861756, -0.805033, 0.831213, -0.87921, 1.0472, -0.885755, 1.0712,
					  -0.944659, 1.2021, -0.966476, 1.34172, -0.951204, 1.39626, -0.885755, 1.53589, -0.857393, 1.71042, -0.872665, 1.72569, -0.909753, 1.91986,
					  -0.859575, 2.03767, -0.870483, 2.08131, -0.872665, 2.09658, -0.922843, 2.19693, -0.925025, 2.23184, -0.86612, 2.34747, -0.842121, 2.32129,
					  -0.785398, 2.28638, -0.783217, 2.27984, -0.73522, 2.16857, -0.698132, -10, 1.88277, -0.375246, 1.8435, -0.407971, 1.77587, -0.370882,
					  1.73006, -0.386154, 1.72569, -0.423242, 1.7017, -0.418879, 1.72569, -0.477784, 1.69515, -0.475602, -10, 1.59916, -0.488692, 1.55116,
					  -0.490874, -10, 1.54025, -0.488692, 1.41154, -0.530144, -10, 1.35918, -0.61741, 1.28064, -0.687223, 1.40499, -0.737402, 1.39626, -0.785398,
					  1.4399, -0.78758, 1.44644, -0.824668, 1.49662, -0.822486, 1.50753, -0.857393, 1.53589, -0.857393, 1.5817, -0.789761, 1.67988, -0.746128,
					  1.8326, -0.724312, 1.95477, -0.7614, 1.95695, -0.785398, 2.09221, -0.815941, 2.02022, -0.833395, 2.03767, -0.870483, -20};

	Polyline *l = 0;
	int start = 0;
	for (int i = 0; lines[i] > -19.999; i++)
	{
		if (lines[i] < -9.999 && lines[i] > -10.001)
		{
			if (l != 0)
			{
				_polylines.push_back(l);
			}
			int points = 0;
			for (int j = i + 1; lines[j] > -9.999; j++)
			{
				points++;
			}
			points /= 2;
			l = new Polyline(points);
			start = i + 1;
		}
		else
		{
			if ((i - start) % 2 == 0)
			{
				l->setLongitude((i - start) / 2, lines[i]);
			}
			else
			{
				l->setLatitude((i - start) / 2, lines[i]);
			}
		}
	}
	_polylines.push_back(l);

	// Load musics
	std::string mus[] = {"GMDEFEND",
					"GMENBASE",
					"GMGEO1",
					"GMGEO2",
					"GMINTER",
					"GMINTRO1",
					"GMINTRO2",
					"GMINTRO3",
					"GMLOSE",
					"GMMARS",
					"GMNEWMAR",
					"GMSTORY",
					"GMTACTIC",
					"GMWIN"};
	std::string exts[] = {"OGG", "MP3", "MID"};
	int tracks[] = {3, 6, 0, 18, 2, 19, 20, 21, 10, 8, 9, 12, 17, 11};

	// Check which music version is available
	bool cat = true;
	GMCatFile *gmcat = 0;

	std::stringstream musDos;
	musDos << folder << "SOUND/GM.CAT";
	struct stat musInfo;
	if (stat(insensitive(musDos.str()).c_str(), &musInfo) == 0)
	{
		cat = true;
		gmcat = new GMCatFile(insensitive(musDos.str()).c_str());
	}
	else
	{
		cat = false;
	}

	for (int i = 0; i < 14; i++)
	{
		if (cat)
		{
			_musics[mus[i]] = gmcat->loadMIDI(tracks[i]);
		}
		else
		{
			_musics[mus[i]] = new Music();
			for (int j = 0; j < 3; j++)
			{
				std::stringstream s;
				s << folder << "SOUND/" << mus[i] << "." << exts[j];
				struct stat info;
				if (stat(insensitive(s.str()).c_str(), &info) == 0) 
				{
					_musics[mus[i]]->load(insensitive(s.str()));
					break;
				}
			}
		}
	}
	delete gmcat;

	// Load sounds
	std::string catsId[] = {"GEO.CAT",
						 "BATTLE.CAT",
						"INTRO.CAT"};
	std::string catsDos[] = {"SOUND2.CAT",
						"SOUND1.CAT",
						"INTRO.CAT"};
	std::string catsWin[] = {"SAMPLE.CAT",
						"SAMPLE2.CAT",
						"SAMPLE3.CAT"};

	// Check which sound version is available
	std::string *cats = 0;
	bool wav = true;

	std::stringstream win, dos;
	win << folder << "SOUND/" << catsWin[0];
	dos << folder << "SOUND/" << catsDos[0];
	struct stat sndInfo;
	if (stat(insensitive(win.str()).c_str(), &sndInfo) == 0)
	{
		cats = catsWin;
		wav = true;
	}
	else if (stat(insensitive(dos.str()).c_str(), &sndInfo) == 0)
	{
		cats = catsDos;
		wav = false;
	}

	for (int i = 0; i < 3; i++)
	{
		if (cats == 0)
		{
			_sounds[catsId[i]] = new SoundSet();
		}
		else
		{
			std::stringstream s;
			s << folder << "SOUND/" << cats[i];
			_sounds[catsId[i]] = new SoundSet();
			_sounds[catsId[i]]->loadCat(insensitive(s.str()), wav);
		}
	}

	TextButton::soundPress = _sounds["GEO.CAT"]->getSound(0);
	Window::soundPopup[0] = _sounds["GEO.CAT"]->getSound(1);
	Window::soundPopup[1] = _sounds["GEO.CAT"]->getSound(2);
	Window::soundPopup[2] = _sounds["GEO.CAT"]->getSound(3);

	loadBattlescapeResources(folder);
}

/**
 *
 */
XcomResourcePack::~XcomResourcePack()
{
}


void XcomResourcePack::loadBattlescapeResources(std::string folder)
{
	// Load Battlescape ICONS
	std::stringstream s;
	s << folder << "UFOGRAPH/" << "ICONS.PCK";
	_surfaces["ICONS.PCK"] = new Surface(320, 200);
	_surfaces["ICONS.PCK"]->loadSpk(insensitive(s.str()));

	s.str("");
	std::stringstream s2;
	s << folder << "UFOGRAPH/" << "CURSOR.PCK";
	s2 << folder << "UFOGRAPH/" << "CURSOR.TAB";
	_sets["CURSOR.PCK"] = new SurfaceSet(32, 40);
	_sets["CURSOR.PCK"]->loadPck(insensitive(s.str()),insensitive(s2.str()));

	// Load Batltescape terrain
	std::string bsets[] = {"AVENGER.PCK",
					  "BARN.PCK",
					  "BLANKS.PCK",
					 "BRAIN.PCK",
					 "CULTIVAT.PCK",
					 "DESERT.PCK",
					 "FOREST.PCK",
					 "FRNITURE.PCK",
					 "JUNGLE.PCK",
					 "LIGHTNIN.PCK",
					 "MARS.PCK",
					 "MOUNT.PCK",
					 "PLANE.PCK",
					 "POLAR.PCK",
					 "ROADS.PCK",
					 "U_BASE.PCK",
					 "U_BITS.PCK",
					 "U_DISEC2.PCK",
					 "U_EXT02.PCK",
					 "U_OPER2.PCK",
					 "U_PODS.PCK",
					 "U_WALL02.PCK",
					 "UFO1.PCK",
					 "URBAN.PCK",
					 "URBITS.PCK",
					 "XBASE1.PCK",
					 "XBASE2.PCK"
					 };

	for (int i = 0; i < 27; i++)
	{
		std::stringstream s;
		s << folder << "TERRAIN/" << bsets[i];

		std::string ext = bsets[i].substr(bsets[i].length()-3, bsets[i].length());
		if (ext == "PCK")
		{
			std::string tab = bsets[i].substr(0, bsets[i].length()-4) + ".TAB";
			std::string mcd = bsets[i].substr(0, bsets[i].length()-4) + ".MCD";
			std::stringstream s2;
			s2 << folder << "TERRAIN/" << tab;
			std::stringstream s3;
			s3 << folder << "TERRAIN/" << mcd;
			_sets[bsets[i]] = new SurfaceSet(32, 40);
			_sets[bsets[i]]->loadPck(insensitive(s.str()), insensitive(s2.str()));
			_terrainSets[mcd] = new TerrainObjectSet(s3.str());
			_terrainSets[mcd]->load(insensitive(s3.str()));
			_terrainSets[mcd]->linkSprites(_sets[bsets[i]]);
		}
		else
		{
			_sets[bsets[i]] = new SurfaceSet(32, 32);
			_sets[bsets[i]]->loadDat(insensitive(s.str()));
		}
	}

		// Load Battlescape units
	std::string usets[] = {"SILACOID.PCK",
					  "CELATID.PCK",
					  "HANDOB.PCK",
					 "CYBER.PCK",
					 "FLOOROB.PCK",
					 "SECTOID.PCK",
					 "CIVF.PCK",
					 "CIVM.PCK",
					 "XCOM_1.PCK",
					 "SNAKEMAN.PCK",
					 "XCOM_0.PCK",
					 "CHRYS.PCK",
					 "TANKS.PCK",
					 "FLOATER.PCK",
					 "XCOM_2.PCK",
					 "ZOMBIE.PCK",
					 "MUTON.PCK",
					 "X_REAP.PCK",
					 "ETHEREAL.PCK",
					 "X_ROB.PCK"
					 };

	for (int i = 0; i < 20; i++)
	{
		std::stringstream s;
		s << folder << "UNITS/" << usets[i];

		std::string ext = usets[i].substr(usets[i].length()-3, usets[i].length());
		if (ext == "PCK")
		{
			std::string tab = usets[i].substr(0, usets[i].length()-4) + ".TAB";
			std::stringstream s2;
			s2 << folder << "UNITS/" << tab;
			_sets[usets[i]] = new SurfaceSet(32, 40);
			_sets[usets[i]]->loadPck(insensitive(s.str()), insensitive(s2.str()));
		}
		else
		{
			_sets[usets[i]] = new SurfaceSet(32, 32);
			_sets[usets[i]]->loadDat(insensitive(s.str()));
		}
	}


}


/**
 * Loads a X-Com format MAP file into the tiles of the battlegame.
 * @param mapblock pointer to MapBlock.
 * @param xoff mapblock offset in X direction
 * @param yoff mapblock offset in Y direction
 * @param save pointer to the current SavedBattleGame
 * @param terrain pointer to the Terrain rule
 * @return int Height of the loaded mapblock (this is needed for spawpoint calculation...)
 * @sa http://www.ufopaedia.org/index.php?title=MAPS
 * NOTE that Y-axis is in reverse order
 */
int XcomResourcePack::loadMAP(MapBlock *mapblock, int xoff, int yoff, SavedBattleGame *save, RuleTerrain *terrain)
{
	int width, length, height;
	int x = xoff, y = yoff, z = 0;
	char size[3];
	char value[4];
	std::stringstream filename;
	filename << _folder << "MAPS/" << mapblock->getName() << ".MAP";
	std::string mapDataFileName;
	int terrainObjectID;

	// Load file
	std::ifstream mapFile (insensitive(filename.str()).c_str(), std::ios::in| std::ios::binary);
	if (!mapFile)
	{
		throw "Failed to load MAP";
	}
	
	mapFile.read((char*)&size, sizeof(size));
	length = (int)size[0];
	width = (int)size[1];
	height = (int)size[2];
	z += height - 1;
	y += length - 1;
	mapblock->setHeight(height);

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		for (int part = 0; part < 4; part++)
		{
			terrainObjectID = (int)value[part];
			if (terrainObjectID>0)
			{
				save->getTile(Position(x, y, z))->setName(terrain->getTerrainObjectName(terrainObjectID),part);
			}
			// if the part is empty and it's not a floor, remove it
			// it prevents growing grass in UFOs
			if (terrainObjectID == 0 && part > 0)
			{
				save->getTile(Position(x, y, z))->setName("",part);
			}
		}

		x++;

		if (x == (width + xoff))
		{
			x = xoff;
			y--;
		}
		if (y == yoff - 1)
		{
			y = length - 1 + yoff;
			z--;
		}
	}

	if (!mapFile.eof())
	{
		throw "Invalid data from file";
	}

	mapFile.close();

	return height;
}

/**
 * Loads a X-Com format RMP file into the spawnpoints of the battlegame.
 * @param mapblock pointer to MapBlock.
 * @param xoff mapblock offset in X direction
 * @param yoff mapblock offset in Y direction
 * @param save pointer to the current battle game
 * @sa http://www.ufopaedia.org/index.php?title=ROUTES
 */
void XcomResourcePack::loadRMP(MapBlock *mapblock, int xoff, int yoff, SavedBattleGame* save)
{
	int id = 0;
	char value[24];
	std::stringstream filename;
	filename << _folder << "ROUTES/" << mapblock->getName() << ".RMP";

	// Load file
	std::ifstream mapFile (insensitive(filename.str()).c_str(), std::ios::in| std::ios::binary);
	if (!mapFile)
	{
		throw "Failed to load RMP";
	}

	int nodeOffset = save->getNodes()->size();

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		Node *node = new Node(nodeOffset + id, xoff + (int)value[1], yoff + (mapblock->getLength() - (int)value[0]), mapblock->getHeight() - 1 - (int)value[2], (int)value[3], (int)value[19], (int)value[20], (int)value[21], (int)value[22], (int)value[23]);
		for (int j=0;j<5;j++)
		{
			int connectID = (int)((signed char)value[4 + j*3]);
			if (connectID > -1)
			{
				connectID += nodeOffset;
			}
			node->assignNodeLink(new NodeLink(connectID, (int)value[5 + j*3], (int)value[6 + j*3]), j);
		}
		save->getNodes()->push_back(node);
		id++;
	}

	if (!mapFile.eof())
	{
		throw "Invalid data from file";
	}

	mapFile.close();
}
