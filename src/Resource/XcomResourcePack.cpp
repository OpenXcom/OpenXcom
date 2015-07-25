/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include <climits>
#include "../Engine/CrossPlatform.h"
#include "../Engine/FileMap.h"
#include "../Engine/Palette.h"
#include "../Engine/Font.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Language.h"
#include "../Engine/Music.h"
#include "../Engine/GMCat.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/Node.h"
#include "../Battlescape/Position.h"
#include "../Ruleset/MapDataSet.h"
#include "../Ruleset/RuleMusic.h"
#include "../Engine/ShaderDraw.h"
#include "../Engine/ShaderMove.h"
#include "../Engine/Exception.h"
#include "../Engine/Logger.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/SoundDefinition.h"
#include "../Ruleset/ExtraSprites.h"
#include "../Ruleset/ExtraSounds.h"
#include "../Engine/AdlibMusic.h"
#include "../fmath.h"

namespace OpenXcom
{

namespace
{

const Uint8 ShadeMax = 15;
/**
 * Recolor class used in UFO
 */
struct HairXCOM1
{
	static const Uint8 Hair = 9 << 4;
	static const Uint8 Face = 6 << 4;
	static inline void func(Uint8& src, const Uint8& cutoff, int, int, int)
	{
		if (src > cutoff && src <= Face + ShadeMax)
		{
			src = Hair + (src & ShadeMax) - 6; //make hair color like male in xcom_0.pck
		}
	}
};

/**
 * Recolor class used in TFTD
 */
struct HairXCOM2
{
	static const Uint8 ManHairColor = 4 << 4;
	static const Uint8 WomanHairColor = 1 << 4;
	static inline void func(Uint8& src, int, int, int, int)
	{
		if (src >= WomanHairColor && src <= WomanHairColor + ShadeMax)
		{
			src = ManHairColor + (src & ShadeMax);
		}
	}
};

/**
 * Recolor class used in TFTD
 */
struct FaceXCOM2
{
	static const Uint8 FaceColor = 10 << 4;
	static const Uint8 PinkColor = 14 << 4;
	static inline void func(Uint8& src, int, int, int, int)
	{
		if (src >= FaceColor && src <= FaceColor + ShadeMax)
		{
			src = PinkColor + (src & ShadeMax);
		}
	}
};

/**
 * Recolor class used in TFTD
 */
struct BodyXCOM2
{
	static const Uint8 IonArmorColor = 8 << 4;
	static inline void func(Uint8& src, int, int, int, int)
	{
		if (src == 153)
		{
			src = IonArmorColor + 12;
		}
		else if (src == 151)
		{
			src = IonArmorColor + 10;
		}
		else if (src == 148)
		{
			src = IonArmorColor + 4;
		}
		else if (src == 147)
		{
			src = IonArmorColor + 2;
		}
		else if (src >= HairXCOM2::WomanHairColor && src <= HairXCOM2::WomanHairColor + ShadeMax)
		{
			src = IonArmorColor + (src & ShadeMax);
		}
	}
};
/**
 * Recolor class used in TFTD
 */
struct FallXCOM2
{
	static const Uint8 RoguePixel = 151;
	static inline void func(Uint8& src, int, int, int, int)
	{
		if (src == RoguePixel)
		{
			src = FaceXCOM2::PinkColor + (src & ShadeMax) + 2;
		}
		else if (src >= BodyXCOM2::IonArmorColor && src <= BodyXCOM2::IonArmorColor + ShadeMax)
		{
			src = FaceXCOM2::PinkColor + (src & ShadeMax);
		}
	}
};


}

/**
 * Initializes the resource pack by loading all the resources
 * contained in the original game folder.
 * @param extraSprites List of mod extra sprites.
 * @param extraSounds List of mod extra sounds.
 */
XcomResourcePack::XcomResourcePack(Ruleset *rules) : ResourcePack()
{
	_ruleset = rules;
	// Load palettes
	const char *pal[] = {"PAL_GEOSCAPE", "PAL_BASESCAPE", "PAL_GRAPHS", "PAL_UFOPAEDIA", "PAL_BATTLEPEDIA"};
	for (size_t i = 0; i < sizeof(pal) / sizeof(pal[0]); ++i)
	{
		std::string s = "GEODATA/PALETTES.DAT";
		_palettes[pal[i]] = new Palette();
		_palettes[pal[i]]->loadDat(FileMap::getFilePath(s), 256, Palette::palOffset(i));
	}
	{
		std::string s1 = "GEODATA/BACKPALS.DAT";
		std::string s2 = "BACKPALS.DAT";
		_palettes[s2] = new Palette();
		_palettes[s2]->loadDat(FileMap::getFilePath(s1), 128);
	}

	// Correct Battlescape palette
	{
		std::string s1 = "GEODATA/PALETTES.DAT";
		std::string s2 = "PAL_BATTLESCAPE";
		_palettes[s2] = new Palette();
		_palettes[s2]->loadDat(FileMap::getFilePath(s1), 256, Palette::palOffset(4));

		// Last 16 colors are a greyish gradient
		SDL_Color gradient[] = {{140, 152, 148, 255},
								{132, 136, 140, 255},
								{116, 124, 132, 255},
								{108, 116, 124, 255},
								{92, 104, 108, 255},
								{84, 92, 100, 255},
								{76, 80, 92, 255},
								{56, 68, 84, 255},
								{48, 56, 68, 255},
								{40, 48, 56, 255},
								{32, 36, 48, 255},
								{24, 28, 32, 255},
								{16, 20, 24, 255},
								{8, 12, 16, 255},
								{3, 4, 8, 255},
								{3, 3, 6, 255}};
		for (size_t i = 0; i < sizeof(gradient)/sizeof(gradient[0]); ++i)
		{
			SDL_Color *color = _palettes[s2]->getColors(Palette::backPos + 16 + i);
			*color = gradient[i];
		}
	}

	// Load fonts
	std::string fontname = rules->getFontName();
	YAML::Node doc = YAML::LoadFile(FileMap::getFilePath("Language/" + fontname));
	Log(LOG_INFO) << "Loading font... " << fontname;
	Font::setIndex(Language::utf8ToWstr(doc["chars"].as<std::string>()));
	for (YAML::const_iterator i = doc["fonts"].begin(); i != doc["fonts"].end(); ++i)
	{
		std::string id = (*i)["id"].as<std::string>();
		Font *font = new Font();
		font->load(*i);
		_fonts[id] = font;
	}

	// Load surfaces
	{
		std::ostringstream s;
		s << "GEODATA/" << "INTERWIN.DAT";
		_surfaces["INTERWIN.DAT"] = new Surface(160, 556);
		_surfaces["INTERWIN.DAT"]->loadScr(FileMap::getFilePath(s.str()));
	}

	const std::set<std::string> &geographFiles(FileMap::getVFolderContents("GEOGRAPH"));
	std::set<std::string> scrs = FileMap::filterFiles(geographFiles, "SCR");
	for (std::set<std::string>::iterator i = scrs.begin(); i != scrs.end(); ++i)
	{
		std::string fname = *i;
		std::transform(i->begin(), i->end(), fname.begin(), toupper);
		_surfaces[fname] = new Surface(320, 200);
		_surfaces[fname]->loadScr(FileMap::getFilePath("GEOGRAPH/" + fname));
	}
	std::set<std::string> bdys = FileMap::filterFiles(geographFiles, "BDY");
	for (std::set<std::string>::iterator i = bdys.begin(); i != bdys.end(); ++i)
	{
		std::string fname = *i;
		std::transform(i->begin(), i->end(), fname.begin(), toupper);
		_surfaces[fname] = new Surface(320, 200);
		_surfaces[fname]->loadBdy(FileMap::getFilePath("GEOGRAPH/" + fname));
	}

	// bigger geoscape background
	int newWidth = 320 - 64, newHeight = 200;
	Surface *newGeo = new Surface(newWidth*3, newHeight*3);
	Surface *oldGeo = _surfaces["GEOBORD.SCR"];
	for (int x = 0; x < newWidth; ++x)
	{
		for (int y = 0; y < newHeight; ++y)
		{
			newGeo->setPixel(newWidth+x, newHeight+y, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth-x-1, newHeight+y, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth*3-x-1, newHeight+y, oldGeo->getPixel(x, y));

			newGeo->setPixel(newWidth+x, newHeight-y-1, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth-x-1, newHeight-y-1, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth*3-x-1, newHeight-y-1, oldGeo->getPixel(x, y));

			newGeo->setPixel(newWidth+x, newHeight*3-y-1, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth-x-1, newHeight*3-y-1, oldGeo->getPixel(x, y));
			newGeo->setPixel(newWidth*3-x-1, newHeight*3-y-1, oldGeo->getPixel(x, y));
		}
	}
	_surfaces["ALTGEOBORD.SCR"] = newGeo;

	// here we create an "alternate" background surface for the base info screen.
	_surfaces["ALTBACK07.SCR"] = new Surface(320, 200);
	_surfaces["ALTBACK07.SCR"]->loadScr(FileMap::getFilePath("GEOGRAPH/BACK07.SCR"));
	for (int y = 172; y >= 152; --y)
		for (int x = 5; x <= 314; ++x)
			_surfaces["ALTBACK07.SCR"]->setPixel(x, y+4, _surfaces["ALTBACK07.SCR"]->getPixel(x,y));
	for (int y = 147; y >= 134; --y)
		for (int x = 5; x <= 314; ++x)
			_surfaces["ALTBACK07.SCR"]->setPixel(x, y+9, _surfaces["ALTBACK07.SCR"]->getPixel(x,y));
	for (int y = 132; y >= 109; --y)
		for (int x = 5; x <= 314; ++x)
			_surfaces["ALTBACK07.SCR"]->setPixel(x, y+10, _surfaces["ALTBACK07.SCR"]->getPixel(x,y));

	std::set<std::string> spks = FileMap::filterFiles(geographFiles, "SPK");
	for (std::set<std::string>::iterator i = spks.begin(); i != spks.end(); ++i)
	{
		std::string fname = *i;
		std::transform(i->begin(), i->end(), fname.begin(), toupper);
		_surfaces[fname] = new Surface(320, 200);
		_surfaces[fname]->loadSpk(FileMap::getFilePath("GEOGRAPH/" + fname));
	}

	// Load surface sets
	std::string sets[] = {"BASEBITS.PCK",
			      "INTICON.PCK",
			      "TEXTURE.DAT"};

	for (size_t i = 0; i < sizeof(sets)/sizeof(sets[0]); ++i)
	{
		std::ostringstream s;
		s << "GEOGRAPH/" << sets[i];

		std::string ext = sets[i].substr(sets[i].find_last_of('.')+1, sets[i].length());
		if (ext == "PCK")
		{
			std::string tab = CrossPlatform::noExt(sets[i]) + ".TAB";
			std::ostringstream s2;
			s2 << "GEOGRAPH/" << tab;
			_sets[sets[i]] = new SurfaceSet(32, 40);
			_sets[sets[i]]->loadPck(FileMap::getFilePath(s.str()), FileMap::getFilePath(s2.str()));
		}
		else
		{
			_sets[sets[i]] = new SurfaceSet(32, 32);
			_sets[sets[i]]->loadDat(FileMap::getFilePath(s.str()));
		}
	}
	_sets["SCANG.DAT"] = new SurfaceSet(4, 4);
	std::ostringstream scang;
	scang << "GEODATA/" << "SCANG.DAT";
	_sets["SCANG.DAT"]->loadDat (FileMap::getFilePath(scang.str()));

	if (!Options::mute)
	{
#ifndef __NO_MUSIC
		// Load musics
		const std::map<std::string, RuleMusic *> musics = *rules->getMusic();

		// Check which music version is available
		CatFile *adlibcat = 0, *aintrocat = 0;
		GMCatFile *gmcat = 0;

		const std::set<std::string> &soundFiles(FileMap::getVFolderContents("SOUND"));
		for (std::set<std::string>::iterator i = soundFiles.begin(); i != soundFiles.end(); ++i)
		{
			if (0 == i->compare("adlib.cat"))
			{
				adlibcat = new CatFile(FileMap::getFilePath("SOUND/" + *i).c_str());
			}
			else if (0 == i->compare("aintro.cat"))
			{
				aintrocat = new CatFile(FileMap::getFilePath("SOUND/" + *i).c_str());
			}
			else if (0 == i->compare("gm.cat"))
			{
				gmcat = new GMCatFile(FileMap::getFilePath("SOUND/" + *i).c_str());
			}
		}

		// Try the preferred format first, otherwise use the default priority
		MusicFormat priority[] = {Options::preferredMusic, MUSIC_FLAC, MUSIC_OGG, MUSIC_MP3, MUSIC_MOD, MUSIC_WAV, MUSIC_ADLIB, MUSIC_MIDI};
		for (std::map<std::string, RuleMusic *>::const_iterator i = musics.begin(); i != musics.end(); ++i)
		{
			Music *music = 0;
			for (size_t j = 0; j < sizeof(priority)/sizeof(priority[0]) && music == 0; ++j)
			{
				music = loadMusic(priority[j], (*i).first, (*i).second->getCatPos(), (*i).second->getNormalization(), adlibcat, aintrocat, gmcat);
			}
			if (music)
			{
				_musics[(*i).first] = music;
			}

		}

		delete gmcat;
		delete adlibcat;
		delete aintrocat;
#endif

		if (rules->getSoundDefinitions()->empty())
		{
			// Load sounds
			std::string catsId[] = {"GEO.CAT", "BATTLE.CAT"};
			std::string catsDos[] = {"SOUND2.CAT", "SOUND1.CAT"};
			std::string catsWin[] = {"SAMPLE.CAT", "SAMPLE2.CAT"};

			// Try the preferred format first, otherwise use the default priority
			std::string *cats[] = {0, catsWin, catsDos};
			if (Options::preferredSound == SOUND_14)
				cats[0] = catsWin;
			else if (Options::preferredSound == SOUND_10)
				cats[1] = catsDos;

			Options::currentSound = SOUND_AUTO;
			for (size_t i = 0; i < sizeof(catsId) / sizeof(catsId[0]); ++i)
			{
				SoundSet *sound = 0;
				for (size_t j = 0; j < sizeof(cats) / sizeof(cats[0]) && sound == 0; ++j)
				{
					bool wav = true;
					if (cats[j] == 0)
						continue;
					else if (cats[j] == catsDos)
						wav = false;
					std::string fname = cats[j][i];
					std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
					std::set<std::string>::iterator file = soundFiles.find(fname);
					if (file != soundFiles.end())
					{
						sound = new SoundSet();
						sound->loadCat(FileMap::getFilePath("SOUND/" + cats[j][i]), wav);
						Options::currentSound = (wav) ? SOUND_14 : SOUND_10;
					}
				}
				if (sound == 0)
				{
					throw Exception(catsWin[i] + " not found");
				}
				else
				{
					_sounds[catsId[i]] = sound;
				}
			}
		}
		else
		{
			for (std::map<std::string, SoundDefinition*>::const_iterator i = rules->getSoundDefinitions()->begin();
				i != rules->getSoundDefinitions()->end(); ++i)
			{
				std::string fname = i->second->getCATFile();
				std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
				std::set<std::string>::iterator file = soundFiles.find(fname);
				if (file != soundFiles.end())
				{
					if (_sounds.find((*i).first) == _sounds.end())
					{
						_sounds[(*i).first] = new SoundSet();
					}
					for (std::vector<int>::const_iterator j = (*i).second->getSoundList().begin(); j != (*i).second->getSoundList().end(); ++j)
					{
						_sounds[(*i).first]->loadCatbyIndex(FileMap::getFilePath("SOUND/" + fname), *j);
					}
				}
				else
				{
					throw Exception(fname + " not found");
				}
			}
		}

		std::set<std::string>::iterator file = soundFiles.find("intro.cat");
		if (file != soundFiles.end())
		{
			SoundSet *s = _sounds["INTRO.CAT"] = new SoundSet();
			s->loadCat(FileMap::getFilePath("SOUND/INTRO.CAT"), false);
		}

		file = soundFiles.find("sample3.cat");
		if (file != soundFiles.end())
		{
			SoundSet *s = _sounds["SAMPLE3.CAT"] = new SoundSet();
			s->loadCat(FileMap::getFilePath("SOUND/SAMPLE3.CAT"), true);
		}
	}

	TextButton::soundPress = getSound("GEO.CAT", ResourcePack::BUTTON_PRESS);
	Window::soundPopup[0] = getSound("GEO.CAT", ResourcePack::WINDOW_POPUP[0]);
	Window::soundPopup[1] = getSound("GEO.CAT", ResourcePack::WINDOW_POPUP[1]);
	Window::soundPopup[2] = getSound("GEO.CAT", ResourcePack::WINDOW_POPUP[2]);

	loadBattlescapeResources(); // TODO load this at battlescape start, unload at battlescape end?


	// we create extra rows on the soldier stat screens by shrinking them all down one pixel.
	// this is done after loading them, but BEFORE loading the extraSprites, in case a modder wants to replace them.

	// first, let's do the base info screen
	// erase the old lines, copying from a +2 offset to account for the dithering
	for (int y = 91; y < 199; y += 12)
		for (int x = 0; x < 149; ++x)
			_surfaces["BACK06.SCR"]->setPixel(x, y, _surfaces["BACK06.SCR"]->getPixel(x,y+2));
	// drawn new lines, use the bottom row of pixels as a basis
	for (int y = 89; y < 199; y += 11)
		for (int x = 0; x < 149; ++x)
			_surfaces["BACK06.SCR"]->setPixel(x, y, _surfaces["BACK06.SCR"]->getPixel(x,199));
	// finally, move the top of the graph up by one pixel, offset for the last iteration again due to dithering.
	for (int y = 72; y < 80; ++y)
		for (int x = 0; x < 320; ++x)
		{
			_surfaces["BACK06.SCR"]->setPixel(x, y, _surfaces["BACK06.SCR"]->getPixel(x,y + (y == 79 ? 2 : 1)));
		}

	// now, let's adjust the battlescape info screen.
	// erase the old lines, no need to worry about dithering on this one.
	for (int y = 39; y < 199; y += 10)
		for (int x = 0; x < 169; ++x)
			_surfaces["UNIBORD.PCK"]->setPixel(x, y, _surfaces["UNIBORD.PCK"]->getPixel(x,30));
	// drawn new lines, use the bottom row of pixels as a basis
	for (int y = 190; y > 37; y -= 9)
		for (int x = 0; x < 169; ++x)
			_surfaces["UNIBORD.PCK"]->setPixel(x, y, _surfaces["UNIBORD.PCK"]->getPixel(x,199));
	// move the top of the graph down by eight pixels to erase the row we don't need (we actually created ~1.8 extra rows earlier)
	for (int y = 37; y > 29; --y)
		for (int x = 0; x < 320; ++x)
		{
			_surfaces["UNIBORD.PCK"]->setPixel(x, y, _surfaces["UNIBORD.PCK"]->getPixel(x,y-8));
			_surfaces["UNIBORD.PCK"]->setPixel(x, y-8, 0);
		}

	Log(LOG_INFO) << "Loading extra resources from ruleset...";
	std::vector< std::pair<std::string, ExtraSprites *> >extraSprites = rules->getExtraSprites();
	for (std::vector< std::pair<std::string, ExtraSprites *> >::const_iterator i = extraSprites.begin(); i != extraSprites.end(); ++i)
	{
		std::string sheetName = i->first;
		ExtraSprites *spritePack = i->second;
		bool subdivision = (spritePack->getSubX() != 0 && spritePack->getSubY() != 0);
		if (spritePack->getSingleImage())
		{
			if (_surfaces.find(sheetName) == _surfaces.end())
			{
				Log(LOG_VERBOSE) << "Creating new single image: " << sheetName;
				_surfaces[sheetName] = new Surface(spritePack->getWidth(), spritePack->getHeight());
			}
			else
			{
				Log(LOG_VERBOSE) << "Adding/Replacing single image: " << sheetName;
				delete _surfaces[sheetName];
				_surfaces[sheetName] = new Surface(spritePack->getWidth(), spritePack->getHeight());
			}
			_surfaces[sheetName]->loadImage(FileMap::getFilePath(spritePack->getSprites()->operator[](0)));
		}
		else
		{
			bool adding = false;
			if (_sets.find(sheetName) == _sets.end())
			{
				Log(LOG_VERBOSE) << "Creating new surface set: " << sheetName;
				adding = true;
				 if (subdivision)
				 {
					_sets[sheetName] = new SurfaceSet(spritePack->getSubX(), spritePack->getSubY());
				 }
				 else
				 {
					_sets[sheetName] = new SurfaceSet(spritePack->getWidth(), spritePack->getHeight());
				 }
			}
			else
			{
				Log(LOG_VERBOSE) << "Adding/Replacing items in surface set: " << sheetName;
			}

			if (subdivision)
			{
				int frames = (spritePack->getWidth() / spritePack->getSubX())*(spritePack->getHeight() / spritePack->getSubY());
				Log(LOG_VERBOSE) << "Subdividing into " << frames << " frames.";
			}

			for (std::map<int, std::string>::iterator j = spritePack->getSprites()->begin(); j != spritePack->getSprites()->end(); ++j)
			{
				int startFrame = j->first;
				std:: string fileName = j->second;
				if (fileName.substr(fileName.length() - 1, 1) == "/")
				{
					Log(LOG_VERBOSE) << "Loading surface set from folder: " << fileName << " starting at frame: " << startFrame;
					int offset = startFrame;
					std::set<std::string> contents = FileMap::getVFolderContents(fileName);
					for (std::set<std::string>::iterator k = contents.begin(); k != contents.end(); ++k)
					{
						if (!isImageFile((*k).substr((*k).length() -4, (*k).length())))
							continue;
						try
						{
							std::string fullPath = FileMap::getFilePath(fileName + *k);
							if (_sets[sheetName]->getFrame(offset))
							{
								Log(LOG_VERBOSE) << "Replacing frame: " << offset;
								_sets[sheetName]->getFrame(offset)->loadImage(fullPath);
							}
							else
							{
								if (adding)
								{
									_sets[sheetName]->addFrame(offset)->loadImage(fullPath);
								}
								else
								{
									Log(LOG_VERBOSE) << "Adding frame: " << offset + spritePack->getModIndex();
									_sets[sheetName]->addFrame(offset + spritePack->getModIndex())->loadImage(fullPath);
								}
							}
							offset++;
						}
						catch (Exception &e)
						{
							Log(LOG_WARNING) << e.what();
						}
					}
				}
				else
				{
					if (spritePack->getSubX() == 0 && spritePack->getSubY() == 0)
					{
						std::string fullPath = FileMap::getFilePath(fileName);
						if (_sets[sheetName]->getFrame(startFrame))
						{
							Log(LOG_VERBOSE) << "Replacing frame: " << startFrame;
							_sets[sheetName]->getFrame(startFrame)->loadImage(fullPath);
						}
						else
						{
							Log(LOG_VERBOSE) << "Adding frame: " << startFrame << ", using index: " << startFrame + spritePack->getModIndex();
							_sets[sheetName]->addFrame(startFrame + spritePack->getModIndex())->loadImage(fullPath);
						}
					}
					else
					{
						Surface *temp = new Surface(spritePack->getWidth(), spritePack->getHeight());
						temp->loadImage(FileMap::getFilePath(spritePack->getSprites()->operator[](startFrame)));
						int xDivision = spritePack->getWidth() / spritePack->getSubX();
						int yDivision = spritePack->getHeight() / spritePack->getSubY();
						int offset = startFrame;

						for (int y = 0; y != yDivision; ++y)
						{
							for (int x = 0; x != xDivision; ++x)
							{
								if (_sets[sheetName]->getFrame(offset))
								{
									Log(LOG_VERBOSE) << "Replacing frame: " << offset;
									_sets[sheetName]->getFrame(offset)->clear();
									// for some reason regular blit() doesn't work here how i want it, so i use this function instead.
									temp->blitNShade(_sets[sheetName]->getFrame(offset), 0 - (x * spritePack->getSubX()), 0 - (y * spritePack->getSubY()), 0);
								}
								else
								{
									if (adding)
									{
										// for some reason regular blit() doesn't work here how i want it, so i use this function instead.
										temp->blitNShade(_sets[sheetName]->addFrame(offset), 0 - (x * spritePack->getSubX()), 0 - (y * spritePack->getSubY()), 0);
									}
									else
									{
										Log(LOG_VERBOSE) << "Adding frame: " << offset + spritePack->getModIndex();
										// for some reason regular blit() doesn't work here how i want it, so i use this function instead.
										temp->blitNShade(_sets[sheetName]->addFrame(offset + spritePack->getModIndex()), 0 - (x * spritePack->getSubX()), 0 - (y * spritePack->getSubY()), 0);
									}
								}
								++offset;
							}
						}
						delete temp;
					}
				}
			}
		}
	}

	// copy constructor doesn't like doing this directly, so let's make a second handobs file the old fashioned way.
	// handob2 is used for all the left handed sprites.
	_sets["HANDOB2.PCK"] = new SurfaceSet(_sets["HANDOB.PCK"]->getWidth(), _sets["HANDOB.PCK"]->getHeight());
	std::map<int, Surface*> *handob = _sets["HANDOB.PCK"]->getFrames();
	for (std::map<int, Surface*>::const_iterator i = handob->begin(); i != handob->end(); ++i)
	{
		Surface *surface1 = _sets["HANDOB2.PCK"]->addFrame(i->first);
		Surface *surface2 = i->second;
		surface1->setPalette(surface2->getPalette());
		surface2->blit(surface1);
	}

	std::vector< std::pair<std::string, ExtraSounds *> >extraSounds = rules->getExtraSounds();
	for (std::vector< std::pair<std::string, ExtraSounds *> >::const_iterator i = extraSounds.begin(); i != extraSounds.end(); ++i)
	{
		std::string setName = i->first;
		ExtraSounds *soundPack = i->second;
		if (_sounds.find(setName) == _sounds.end())
		{
			Log(LOG_VERBOSE) << "Creating new sound set: " << setName << ", this will likely have no in-game use.";
			_sounds[setName] = new SoundSet();
		}
		else Log(LOG_VERBOSE) << "Adding/Replacing items in sound set: " << setName;
		for (std::map<int, std::string>::iterator j = soundPack->getSounds()->begin(); j != soundPack->getSounds()->end(); ++j)
		{
			int startSound = j->first;
			std::string fileName = j->second;
			if (fileName.substr(fileName.length() - 1, 1) == "/")
			{
				Log(LOG_VERBOSE) << "Loading sound set from folder: " << fileName << " starting at index: " << startSound;
				int offset = startSound;
				std::set<std::string> contents = FileMap::getVFolderContents(fileName);
				for (std::set<std::string>::iterator k = contents.begin(); k != contents.end(); ++k)
				{
					try
					{
						std::string fullPath = FileMap::getFilePath(fileName + *k);
						if (_sounds[setName]->getSound(offset))
						{
							_sounds[setName]->getSound(offset)->load(fullPath);
						}
						else
						{
							_sounds[setName]->addSound(offset + soundPack->getModIndex())->load(fullPath);
						}
						offset++;
					}
					catch (Exception &e)
					{
						Log(LOG_WARNING) << e.what();
					}
				}
			}
			else
			{
				std::string fullPath = FileMap::getFilePath(fileName);
				if (_sounds[setName]->getSound(startSound))
				{
					Log(LOG_VERBOSE) << "Replacing index: " << startSound;
					_sounds[setName]->getSound(startSound)->load(fullPath);
				}
				else
				{
					Log(LOG_VERBOSE) << "Adding index: " << startSound;
					_sounds[setName]->addSound(startSound + soundPack->getModIndex())->load(fullPath);
				}
			}
		}
	}
}

/**
 *
 */
XcomResourcePack::~XcomResourcePack()
{
}

/**
 * Loads the resources required by the Battlescape.
 */
void XcomResourcePack::loadBattlescapeResources()
{
	// Load Battlescape ICONS
	_sets["SPICONS.DAT"] = new SurfaceSet(32, 24);
	_sets["SPICONS.DAT"]->loadDat(FileMap::getFilePath("UFOGRAPH/SPICONS.DAT"));
	_sets["CURSOR.PCK"] = new SurfaceSet(32, 40);
	_sets["CURSOR.PCK"]->loadPck(FileMap::getFilePath("UFOGRAPH/CURSOR.PCK"), FileMap::getFilePath("UFOGRAPH/CURSOR.TAB"));
	_sets["SMOKE.PCK"] = new SurfaceSet(32, 40);
	_sets["SMOKE.PCK"]->loadPck(FileMap::getFilePath("UFOGRAPH/SMOKE.PCK"), FileMap::getFilePath("UFOGRAPH/SMOKE.TAB"));
	_sets["HIT.PCK"] = new SurfaceSet(32, 40);
	_sets["HIT.PCK"]->loadPck(FileMap::getFilePath("UFOGRAPH/HIT.PCK"), FileMap::getFilePath("UFOGRAPH/HIT.TAB"));
	_sets["X1.PCK"] = new SurfaceSet(128, 64);
	_sets["X1.PCK"]->loadPck(FileMap::getFilePath("UFOGRAPH/X1.PCK"), FileMap::getFilePath("UFOGRAPH/X1.TAB"));
	_sets["MEDIBITS.DAT"] = new SurfaceSet(52, 58);
	_sets["MEDIBITS.DAT"]->loadDat(FileMap::getFilePath("UFOGRAPH/MEDIBITS.DAT"));
	_sets["DETBLOB.DAT"] = new SurfaceSet(16, 16);
	_sets["DETBLOB.DAT"]->loadDat(FileMap::getFilePath("UFOGRAPH/DETBLOB.DAT"));

	// Load Battlescape Terrain (only blanks are loaded, others are loaded just in time)
	_sets["BLANKS.PCK"] = new SurfaceSet(32, 40);
	_sets["BLANKS.PCK"]->loadPck(FileMap::getFilePath("TERRAIN/BLANKS.PCK"), FileMap::getFilePath("TERRAIN/BLANKS.TAB"));

	// Load Battlescape units
	std::set<std::string> unitsContents = FileMap::getVFolderContents("UNITS");
	std::set<std::string> usets = FileMap::filterFiles(unitsContents, "PCK");
	for (std::set<std::string>::iterator i = usets.begin(); i != usets.end(); ++i)
	{
		std::string path = FileMap::getFilePath("UNITS/" + *i);
		std::string tab = FileMap::getFilePath("UNITS/" + CrossPlatform::noExt(*i) + ".TAB");
		std::string fname = *i;
		std::transform(i->begin(), i->end(), fname.begin(), toupper);
		if (fname != "BIGOBS.PCK")
			_sets[fname] = new SurfaceSet(32, 40);
		else
			_sets[fname] = new SurfaceSet(32, 48);
		_sets[fname]->loadPck(path, tab);
	}
	// incomplete chryssalid set: 1.0 data: stop loading.
	if (_sets.find("CHRYS.PCK") != _sets.end() && !_sets["CHRYS.PCK"]->getFrame(225))
	{
		Log(LOG_FATAL) << "Version 1.0 data detected";
		throw Exception("Invalid CHRYS.PCK, please patch your X-COM data to the latest version");
	}
	// TFTD uses the loftemps dat from the terrain folder, but still has enemy unknown's version in the geodata folder, which is short by 2 entries.
	std::set<std::string> terrainContents = FileMap::getVFolderContents("TERRAIN");
	if (terrainContents.find("loftemps.dat") != terrainContents.end())
	{
		MapDataSet::loadLOFTEMPS(FileMap::getFilePath("TERRAIN/LOFTEMPS.DAT"), &_voxelData);
	}
	else
	{
		MapDataSet::loadLOFTEMPS(FileMap::getFilePath("GEODATA/LOFTEMPS.DAT"), &_voxelData);
	}

	std::string scrs[] = {"TAC00.SCR"};

	for (size_t i = 0; i < sizeof(scrs)/sizeof(scrs[0]); ++i)
	{
		_surfaces[scrs[i]] = new Surface(320, 200);
		_surfaces[scrs[i]]->loadScr(FileMap::getFilePath("UFOGRAPH/" + scrs[i]));
	}

	// lower case so we can find them in the contents map
	std::string lbms[] = {"d0.lbm",
						  "d1.lbm",
						  "d2.lbm",
						  "d3.lbm"};
	std::string pals[] = {"PAL_BATTLESCAPE",
						  "PAL_BATTLESCAPE_1",
						  "PAL_BATTLESCAPE_2",
						  "PAL_BATTLESCAPE_3"};

	SDL_Color backPal[] =  {{0, 5, 4, 255},
							{0, 10, 34, 255},
							{2, 9, 24, 255},
							{2, 0, 24, 255}};

	std::set<std::string> ufographContents = FileMap::getVFolderContents("UFOGRAPH");
	for (size_t i = 0; i < sizeof(lbms)/sizeof(lbms[0]); ++i)
	{
		if (ufographContents.find(lbms[i]) == ufographContents.end())
		{
			continue;
		}

		if (!i)
		{
			delete _palettes["PAL_BATTLESCAPE"];
		}

		Surface *tempSurface = new Surface(1, 1);
		tempSurface->loadImage(FileMap::getFilePath("UFOGRAPH/" + lbms[i]));
		_palettes[pals[i]] = new Palette();
		SDL_Color *colors = tempSurface->getPalette();
		colors[255] = backPal[i];
		_palettes[pals[i]]->setColors(colors, 256);
		createTransparencyLUT(_palettes[pals[i]]);
		delete tempSurface;
	}

	std::string spks[] = {"TAC01.SCR",
						  "DETBORD.PCK",
						  "DETBORD2.PCK",
						  "ICONS.PCK",
						  "MEDIBORD.PCK",
						  "SCANBORD.PCK",
						  "UNIBORD.PCK"};

	for (size_t i = 0; i < sizeof(spks)/sizeof(spks[0]); ++i)
	{
		std::string fname = spks[i];
		std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
		if (ufographContents.find(fname) == ufographContents.end())
		{
			continue;
		}

		_surfaces[spks[i]] = new Surface(320, 200);
		_surfaces[spks[i]]->loadSpk(FileMap::getFilePath("UFOGRAPH/" + spks[i]));
	}


	std::set<std::string> bdys = FileMap::filterFiles(ufographContents, "BDY");
	for (std::set<std::string>::iterator i = bdys.begin(); i != bdys.end(); ++i)
	{
		std::string idxName = *i;
		std::transform(i->begin(), i->end(), idxName.begin(), toupper);
		idxName = idxName.substr(0, idxName.length() - 3);
		if (idxName.substr(0, 3) == "MAN")
		{
			idxName = idxName + "SPK";
		}
		else if (idxName == "TAC01.")
		{
			idxName = idxName + "SCR";
		}
		else
		{
			idxName = idxName + "PCK";
		}
		_surfaces[idxName] = new Surface(320, 200);
		_surfaces[idxName]->loadBdy(FileMap::getFilePath("UFOGRAPH/" + *i));
	}

	// Load Battlescape inventory
	std::set<std::string> invs = FileMap::filterFiles(ufographContents, "SPK");
	for (std::set<std::string>::iterator i = invs.begin(); i != invs.end(); ++i)
	{
		std::string fname = *i;
		std::transform(i->begin(), i->end(), fname.begin(), toupper);
		_surfaces[fname] = new Surface(320, 200);
		_surfaces[fname]->loadSpk(FileMap::getFilePath("UFOGRAPH/" + fname));
	}

	//"fix" of color index in original solders sprites
	if (Options::battleHairBleach)
	{
		std::string name;

		//personal armor
		name = "XCOM_1.PCK";
		if (_sets.find(name) != _sets.end())
		{
			SurfaceSet *xcom_1 = _sets[name];

			for (int i = 0; i < 8; ++i)
			{
				//chest frame
				Surface *surf = xcom_1->getFrame(4 * 8 + i);
				ShaderMove<Uint8> head = ShaderMove<Uint8>(surf);
				GraphSubset dim = head.getBaseDomain();
				surf->lock();
				dim.beg_y = 6;
				dim.end_y = 9;
				head.setDomain(dim);
				ShaderDraw<HairXCOM1>(head, ShaderScalar<Uint8>(HairXCOM1::Face + 5));
				dim.beg_y = 9;
				dim.end_y = 10;
				head.setDomain(dim);
				ShaderDraw<HairXCOM1>(head, ShaderScalar<Uint8>(HairXCOM1::Face + 6));
				surf->unlock();
			}

			for (int i = 0; i < 3; ++i)
			{
				//fall frame
				Surface *surf = xcom_1->getFrame(264 + i);
				ShaderMove<Uint8> head = ShaderMove<Uint8>(surf);
				GraphSubset dim = head.getBaseDomain();
				dim.beg_y = 0;
				dim.end_y = 24;
				dim.beg_x = 11;
				dim.end_x = 20;
				head.setDomain(dim);
				surf->lock();
				ShaderDraw<HairXCOM1>(head, ShaderScalar<Uint8>(HairXCOM1::Face + 6));
				surf->unlock();
			}
		}

		//all TFDT armors
		name = "TDXCOM_?.PCK";
		for (int j = 0; j < 3; ++j)
		{
			name[7] = '0' + j;
			if (_sets.find(name) != _sets.end())
			{
				SurfaceSet *xcom_2 = _sets[name];
				for (int i = 0; i < 16; ++i)
				{
					//chest frame without helm
					Surface *surf = xcom_2->getFrame(262 + i);
					surf->lock();
					if (i < 8)
					{
						//female chest frame
						ShaderMove<Uint8> head = ShaderMove<Uint8>(surf);
						GraphSubset dim = head.getBaseDomain();
						dim.beg_y = 6;
						dim.end_y = 18;
						head.setDomain(dim);
						ShaderDraw<HairXCOM2>(head);

						if (j == 2)
						{
							//fix some pixels in ION armor that was overwrite by previous function
							if (i == 0)
							{
								surf->setPixel(18, 14, 16);
							}
							else if (i == 3)
							{
								surf->setPixel(19, 12, 20);
							}
							else if (i == 6)
							{
								surf->setPixel(13, 14, 16);
							}
						}
					}

					//we change face to pink, to prevent mixup with ION armor backpack that have same color group.
					ShaderDraw<FaceXCOM2>(ShaderMove<Uint8>(surf));
					surf->unlock();
				}

				for (int i = 0; i < 2; ++i)
				{
					//fall frame (first and second)
					Surface *surf = xcom_2->getFrame(256 + i);
					surf->lock();

					ShaderMove<Uint8> head = ShaderMove<Uint8>(surf);
					GraphSubset dim = head.getBaseDomain();
					dim.beg_y = 0;
					if (j == 3)
					{
						dim.end_y = 11 + 5 * i;
					}
					else
					{
						dim.end_y = 17;
					}
					head.setDomain(dim);
					ShaderDraw<FallXCOM2>(head);

					//we change face to pink, to prevent mixup with ION armor backpack that have same color group.
					ShaderDraw<FaceXCOM2>(ShaderMove<Uint8>(surf));
					surf->unlock();
				}

				//Palette fix for ION armor
				if (j == 2)
				{
					int size = xcom_2->getTotalFrames();
					for (int i = 0; i < size; ++i)
					{
						Surface *surf = xcom_2->getFrame(i);
						surf->lock();
						ShaderDraw<BodyXCOM2>(ShaderMove<Uint8>(surf));
						surf->unlock();
					}
				}
			}
		}
	}
}

/**
 * Determines if an image file is an acceptable format for the game.
 * @param extension Image file extension.
 * @return True/false
 */
bool XcomResourcePack::isImageFile(std::string extension)
{
	std::transform(extension.begin(), extension.end(), extension.begin(), toupper);

	return (
		// arbitrary limitation: let's not use these ones (although they're officially supported by sdl)
		/*
		extension == ".ICO" ||
		extension == ".CUR" ||
		extension == ".PNM" ||
		extension == ".PPM" ||
		extension == ".PGM" ||
		extension == ".PBM" ||
		extension == ".XPM" ||
		extension == "ILBM" ||
		// excluding jpeg to avoid inevitable issues due to compression
		extension == ".JPG" ||
		extension == "JPEG" ||
		*/
		extension == ".BMP" ||
		extension == ".LBM" ||
		extension == ".IFF" ||
		extension == ".PCX" ||
		extension == ".GIF" ||
		extension == ".PNG" ||
		extension == ".TGA" ||
		extension == ".TIF" ||
		extension == "TIFF");
}

/**
 * Loads the specified music file format.
 * @param fmt Format of the music.
 * @param file Filename of the music.
 * @param track Track number of the music, if stored in a CAT.
 * @param volume Volume modifier of the music, if stored in a CAT.
 * @param adlibcat Pointer to ADLIB.CAT if available.
 * @param aintrocat Pointer to AINTRO.CAT if available.
 * @param gmcat Pointer to GM.CAT if available.
 * @return Pointer to the music file, or NULL if it couldn't be loaded.
 */
Music *XcomResourcePack::loadMusic(MusicFormat fmt, const std::string &file, int track, float volume, CatFile *adlibcat, CatFile *aintrocat, GMCatFile *gmcat)
{
	/* MUSIC_AUTO, MUSIC_FLAC, MUSIC_OGG, MUSIC_MP3, MUSIC_MOD, MUSIC_WAV, MUSIC_ADLIB, MUSIC_MIDI */
	static const std::string exts[] = {"", ".flac", ".ogg", ".mp3", ".mod", ".wav", "", ".mid"};
	Music *music = 0;
	std::set<std::string> soundContents = FileMap::getVFolderContents("SOUND");
	try
	{
		std::string fname = file + exts[fmt];
		std::transform(fname.begin(), fname.end(), fname.begin(), tolower);

		// Try Adlib music
		if (fmt == MUSIC_ADLIB)
		{
			if (adlibcat && Options::audioBitDepth == 16)
			{
				music = new AdlibMusic(volume);
				if (track < adlibcat->getAmount())
				{
					music->load(adlibcat->load(track, true), adlibcat->getObjectSize(track));
				}
				// separate intro music
				else if (aintrocat)
				{
					track -= adlibcat->getAmount();
					if (track < aintrocat->getAmount())
					{
						music->load(aintrocat->load(track, true), aintrocat->getObjectSize(track));
					}
					else
					{
						delete music;
						music = 0;
					}
				}
			}
		}
		// Try MIDI music
		else if (fmt == MUSIC_MIDI)
		{
			// DOS MIDI
			if (gmcat && track < gmcat->getAmount())
			{
				music = gmcat->loadMIDI(track);
			}
			// Windows MIDI
			else
			{
				if (soundContents.find(fname) != soundContents.end())
				{
					music = new Music();
					music->load(FileMap::getFilePath("SOUND/" + fname));
				}
			}
		}
		// Try digital tracks
		else
		{
			if (soundContents.find(fname) != soundContents.end())
			{
				music = new Music();
				music->load(FileMap::getFilePath("SOUND/" + fname));
			}
		}
	}
	catch (Exception &e)
	{
		Log(LOG_INFO) << e.what();
		if (music) delete music;
		music = 0;
	}
	return music;
}

/**
 * Preamble:
 * this is the most horrible function i've ever written, and it makes me sad.
 * this is, however, a necessary evil, in order to save massive amounts of time in the draw function.
 * when used with the default TFTD ruleset, this function loops 4,194,304 times
 * (4 palettes, 4 tints, 4 levels of opacity, 256 colors, 256 comparisons per)
 * each additional tint in the rulesets will result in over a million iterations more.
 * @param pal the palette to base the lookup table on.
 */
void XcomResourcePack::createTransparencyLUT(Palette *pal)
{
	SDL_Color desiredColor;
	std::vector<Uint8> lookUpTable;
	// start with the color sets
	for (std::vector<SDL_Color>::const_iterator tint = _ruleset->getTransparencies()->begin(); tint != _ruleset->getTransparencies()->end(); ++tint)
	{
		// then the opacity levels, using the alpha channel as the step
		for (int opacity = 1; opacity < 1 + tint->unused * 4; opacity += tint->unused)
		{
			// then the palette itself
			for (int currentColor = 0; currentColor < 256; ++currentColor)
			{
				// add the RGB values from the ruleset to those of the colors contained in the palette
				// in order to determine the desired color
				// yes all this casting and clamping is required, we're dealing with Uint8s here, and there's
				// a lot of potential for values to wrap around, which would be very bad indeed.
				desiredColor.r = std::min(255, (int)(pal->getColors(currentColor)->r) + (tint->r * opacity));
				desiredColor.g = std::min(255, (int)(pal->getColors(currentColor)->g) + (tint->g * opacity));
				desiredColor.b = std::min(255, (int)(pal->getColors(currentColor)->b) + (tint->b * opacity));

				Uint8 closest = 0;
				int lowestDifference = INT_MAX;
				// now compare each color in the palette to find the closest match to our desired one
				for (int comparator = 0; comparator < 256; ++comparator)
				{
					int currentDifference = Sqr(desiredColor.r - pal->getColors(comparator)->r) +
											Sqr(desiredColor.g-pal->getColors(comparator)->g) +
											Sqr(desiredColor.b-pal->getColors(comparator)->b);

					if (currentDifference < lowestDifference)
					{
						closest = comparator;
						lowestDifference = currentDifference;
					}
				}
				lookUpTable.push_back(closest);
			}
		}
	}
	_transparencyLUTs.push_back(lookUpTable);
}

}
