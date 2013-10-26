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
#include "XcomResourcePack.h"
#include <sstream>
#include "../Engine/CrossPlatform.h"
#include "../Engine/Palette.h"
#include "../Engine/Font.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Language.h"
#include "../Engine/Music.h"
#include "../Engine/GMCat.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Engine/Options.h"
#include "../Geoscape/Globe.h"
#include "../Geoscape/Polygon.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/Node.h"
#include "../Battlescape/Position.h"
#include "../Ruleset/MapDataSet.h"
#include "../Engine/ShaderDraw.h"
#include "../Engine/ShaderMove.h"
#include "../Engine/Exception.h"
#include "../Engine/Logger.h"
#include "../Ruleset/ExtraSprites.h"
#include "../Ruleset/ExtraSounds.h"

namespace OpenXcom
{

namespace
{
	
struct HairBleach
{
	static const Uint8 ColorGroup = 15<<4;
	static const Uint8 ColorShade = 15;

	static const Uint8 Hair = 9 << 4;
	static const Uint8 Face = 6 << 4;
	static inline void func(Uint8& src, const Uint8& cutoff, int, int, int)
	{
		if(src > cutoff && src <= Face + 15)
		{
			src = Hair + (src & ColorShade) - 6; //make hair color like male in xcom_0.pck
		}
	}
};

}
	
/**
 * Initializes the resource pack by loading all the resources
 * contained in the original game folder.
 */
XcomResourcePack::XcomResourcePack(std::vector<std::pair<std::string, ExtraSprites *> > extraSprites, std::vector<std::pair<std::string, ExtraSounds *> > extraSounds) : ResourcePack()
{
	// Load palettes
	for (int i = 0; i < 5; ++i)
	{
		std::stringstream s1, s2;
		s1 << "GEODATA/PALETTES.DAT";
		s2 << "PALETTES.DAT_" << i;
		_palettes[s2.str()] = new Palette();
		_palettes[s2.str()]->loadDat(CrossPlatform::getDataFile(s1.str()), 256, Palette::palOffset(i));
	}

	std::stringstream s1, s2;
	s1 << "GEODATA/BACKPALS.DAT";
	s2 << "BACKPALS.DAT";
	_palettes[s2.str()] = new Palette();
	_palettes[s2.str()]->loadDat(CrossPlatform::getDataFile(s1.str()), 128);

	// Load fonts
	YAML::Node doc = YAML::LoadFile(CrossPlatform::getDataFile("Language/Font.dat"));
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
		std::stringstream s;
		s << "GEODATA/" << "INTERWIN.DAT";
		_surfaces["INTERWIN.DAT"] = new Surface(160, 556);
		_surfaces["INTERWIN.DAT"]->loadScr(CrossPlatform::getDataFile(s.str()));
	}

	std::string geograph = CrossPlatform::getDataFolder("GEOGRAPH/");
	std::vector<std::string> scrs = CrossPlatform::getFolderContents(geograph, "SCR");
	for (std::vector<std::string>::iterator i = scrs.begin(); i != scrs.end(); ++i)
	{
		std::string path = geograph + *i;
		std::transform(i->begin(), i->end(), i->begin(), toupper);
		_surfaces[*i] = new Surface(320, 200);
		_surfaces[*i]->loadScr(path);
	}

	// here we create an "alternate" background surface for the base info screen.
	_surfaces["ALTBACK07.SCR"] = new Surface(320, 200);
	_surfaces["ALTBACK07.SCR"]->loadScr(CrossPlatform::getDataFile("GEOGRAPH/BACK07.SCR"));
	for (int y = 172; y >= 152; --y)
		for (int x = 5; x <= 314; ++x)
			_surfaces["ALTBACK07.SCR"]->setPixel(x, y+4, _surfaces["ALTBACK07.SCR"]->getPixel(x,y));
	for (int y = 147; y >= 134; --y)
		for (int x = 5; x <= 314; ++x)
			_surfaces["ALTBACK07.SCR"]->setPixel(x, y+9, _surfaces["ALTBACK07.SCR"]->getPixel(x,y));
	for (int y = 132; y >= 109; --y)
		for (int x = 5; x <= 314; ++x)
			_surfaces["ALTBACK07.SCR"]->setPixel(x, y+10, _surfaces["ALTBACK07.SCR"]->getPixel(x,y));

	std::vector<std::string> spks = CrossPlatform::getFolderContents(geograph, "SPK");
	for (std::vector<std::string>::iterator i = spks.begin(); i != spks.end(); ++i)
	{
		std::string path = geograph + *i;
		std::transform(i->begin(), i->end(), i->begin(), toupper);
		_surfaces[*i] = new Surface(320, 200);
		_surfaces[*i]->loadSpk(path);
	}

	// Load intro
	std::string ufointro = CrossPlatform::getDataFolder("UFOINTRO/");
	std::vector<std::string> lbms = CrossPlatform::getFolderContents(ufointro, "LBM");
	for (std::vector<std::string>::iterator i = lbms.begin(); i != lbms.end(); ++i)
	{
		std::string path = ufointro + *i;
		std::transform(i->begin(), i->end(), i->begin(), toupper);
		_surfaces[*i] = new Surface(320, 200);
		_surfaces[*i]->loadImage(path);
	}

	// Load surface sets
	std::string sets[] = {"BASEBITS.PCK",
						  "INTICON.PCK",
						  "TEXTURE.DAT"};

	for (int i = 0; i < 3; ++i)
	{
		std::stringstream s;
		s << "GEOGRAPH/" << sets[i];

		std::string ext = sets[i].substr(sets[i].find_last_of('.')+1, sets[i].length());
		if (ext == "PCK")
		{
			std::string tab = CrossPlatform::noExt(sets[i]) + ".TAB";
			std::stringstream s2;
			s2 << "GEOGRAPH/" << tab;
			_sets[sets[i]] = new SurfaceSet(32, 40);
			_sets[sets[i]]->loadPck(CrossPlatform::getDataFile(s.str()), CrossPlatform::getDataFile(s2.str()));
		}
		else
		{
			_sets[sets[i]] = new SurfaceSet(32, 32);
			_sets[sets[i]]->loadDat(CrossPlatform::getDataFile(s.str()));
		}
	}
	_sets["SCANG.DAT"] = new SurfaceSet(4, 4);
	std::stringstream scang;
	scang << "GEODATA/" << "SCANG.DAT";
	_sets["SCANG.DAT"]->loadDat (CrossPlatform::getDataFile(scang.str()));
	// Load polygons
	std::stringstream s;
	s << "GEODATA/" << "WORLD.DAT";
	Globe::loadDat(CrossPlatform::getDataFile(s.str()), &_polygons);

	if (!Options::getBool("mute"))
	{
		// Load musics
		std::string mus[] = {"GMDEFEND",
							 "GMENBASE",
							 "GMGEO1",
							 "GMGEO2",
							 "GMGEO3",
							 "GMGEO4",
							 "GMINTER",
							 "GMINTRO1",
							 "GMINTRO2",
							 "GMINTRO3",
							 "GMLOSE",
							 "GMMARS",
							 "GMNEWMAR",
							 "GMSTORY",
							 "GMTACTIC",
							 "GMTACTIC2",
							 "GMWIN"};
		std::string exts[] = {"flac", "ogg", "mp3", "mod"};
		int tracks[] = {3, 6, 0, 18, -1, -1, 2, 19, 20, 21, 10, 9, 8, 12, 17, -1, 11};

#ifndef __NO_MUSIC
		// Check which music version is available
		bool cat = true;
		GMCatFile *gmcat = 0;

		std::string musDos = "SOUND/GM.CAT";
		if (CrossPlatform::fileExists(CrossPlatform::getDataFile(musDos)))
		{
			cat = true;
			gmcat = new GMCatFile(CrossPlatform::getDataFile(musDos).c_str());
		}
		else
		{
			cat = false;
		}

		for (int i = 0; i < 17; ++i)
		{
			bool loaded = false;
			// Try digital tracks
			for (int j = 0; j < 3; ++j)
			{
				std::stringstream s;
				s << "SOUND/" << mus[i] << "." << exts[j];
				if (CrossPlatform::fileExists(CrossPlatform::getDataFile(s.str())))
				{
					_musics[mus[i]] = new Music();
					_musics[mus[i]]->load(CrossPlatform::getDataFile(s.str()));
					loaded = true;
					break;
				}
			}
			if (!loaded)
			{
				// Try Adlib music
				if (cat && tracks[i] != -1)
				{
					_musics[mus[i]] = gmcat->loadMIDI(tracks[i]);
					loaded = true;
				}
				// Try MIDI music
				else
				{
					std::stringstream s;
					s << "SOUND/" << mus[i] << ".mid";
					if (CrossPlatform::fileExists(CrossPlatform::getDataFile(s.str())))
					{
						_musics[mus[i]] = new Music();
						_musics[mus[i]]->load(CrossPlatform::getDataFile(s.str()));
						loaded = true;
					}
				}
			}
			if (!loaded && tracks[i] != -1)
			{
				throw Exception(mus[i] + " not found");
			}
		}
		delete gmcat;
#endif		
		
		// Load sounds
		std::string catsId[] = {"GEO.CAT",
								"BATTLE.CAT"};
		std::string catsDos[] = {"SOUND2.CAT",
								"SOUND1.CAT"};
		std::string catsWin[] = {"SAMPLE.CAT",
								"SAMPLE2.CAT"};

		// Check which sound version is available
		std::string *cats = 0;
		bool wav = true;

		std::stringstream win, dos;
		win << "SOUND/" << catsWin[0];
		dos << "SOUND/" << catsDos[0];
		if (CrossPlatform::fileExists(CrossPlatform::getDataFile(win.str())))
		{
			cats = catsWin;
			wav = true;
		}
		else if (CrossPlatform::fileExists(CrossPlatform::getDataFile(dos.str())))
		{
			cats = catsDos;
			wav = false;
		}

		for (int i = 0; i < 2; ++i)
		{
			if (cats == 0)
			{
				std::stringstream ss;
				ss << catsDos[i] << " not found";
				throw Exception(ss.str());
			}
			else
			{
				std::stringstream s;
				s << "SOUND/" << cats[i];
				_sounds[catsId[i]] = new SoundSet();
				_sounds[catsId[i]]->loadCat(CrossPlatform::getDataFile(s.str()), wav);
			}
		}
		
		if (CrossPlatform::fileExists(CrossPlatform::getDataFile("SOUND/INTRO.CAT")))
		{
			SoundSet *s = _sounds["INTRO.CAT"] = new SoundSet();
			s->loadCat(CrossPlatform::getDataFile("SOUND/INTRO.CAT"), false);
		} else
		{
			Log(LOG_WARNING) << "INTRO.CAT is missing! :(";
		}

		if (CrossPlatform::fileExists(CrossPlatform::getDataFile("SOUND/SAMPLE3.CAT")))
		{
			SoundSet *s = _sounds["SAMPLE3.CAT"] = new SoundSet();
			wav = true;
			s->loadCat(CrossPlatform::getDataFile("SOUND/SAMPLE3.CAT"), true);
		} else
		{
			Log(LOG_WARNING) << "SAMPLE3.CAT is missing! :(";
		}
		
	}

	TextButton::soundPress = getSound("GEO.CAT", 0);
	Window::soundPopup[0] = getSound("GEO.CAT", 1);
	Window::soundPopup[1] = getSound("GEO.CAT", 2);
	Window::soundPopup[2] = getSound("GEO.CAT", 3);

	loadBattlescapeResources(); // TODO load this at battlescape start, unload at battlescape end?
	
	Log(LOG_INFO) << "Loading extra resources from ruleset...";
	bool debugOutput = Options::getBool("debug");
	
	for (std::vector<std::pair<std::string, ExtraSprites *> >::const_iterator i = extraSprites.begin(); i != extraSprites.end(); ++i)
	{
		std::string sheetName = i->first;
		ExtraSprites *spritePack = i->second;
		bool subdivision = (spritePack->getSubX() != 0 && spritePack->getSubY() != 0);
		if (spritePack->getSingleImage())
		{
			if (_surfaces.find(sheetName) == _surfaces.end())
			{
				if (debugOutput)
				{
					Log(LOG_INFO) << "Creating new single image: " << sheetName;
				}
				_surfaces[sheetName] = new Surface(spritePack->getWidth(), spritePack->getHeight());
			}
			else
			{
				if (debugOutput)
				{
					Log(LOG_INFO) << "Adding/Replacing single image: " << sheetName;
				}
				delete _surfaces[sheetName];
				_surfaces[sheetName] = new Surface(spritePack->getWidth(), spritePack->getHeight());
			}
			s.str("");
			s << CrossPlatform::getDataFile(spritePack->getSprites()->operator[](0));
			_surfaces[sheetName]->loadImage(s.str());
		}
		else
		{
			bool adding = false;
			if (_sets.find(sheetName) == _sets.end())
			{
				if (debugOutput)
				{
					Log(LOG_INFO) << "Creating new surface set: " << sheetName;
				}
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
			else if (debugOutput)
			{
				Log(LOG_INFO) << "Adding/Replacing items in surface set: " << sheetName;
			}
			
			if (subdivision && debugOutput)
			{
				int frames = (spritePack->getWidth() / spritePack->getSubX())*(spritePack->getHeight() / spritePack->getSubY());
				Log(LOG_INFO) << "Subdividing into " << frames << " frames.";
			}

			for (std::map<int, std::string>::iterator j = spritePack->getSprites()->begin(); j != spritePack->getSprites()->end(); ++j)
			{
				int startFrame = j->first;
				std:: string fileName = j->second;
				s.str("");
				if (fileName.substr(fileName.length() - 1, 1) == "/")
				{
					if (debugOutput)
					{
						Log(LOG_INFO) << "Loading surface set from folder: " << fileName << " starting at frame: " << startFrame;
					}
					int offset = startFrame;
					std::stringstream folder;
					folder << CrossPlatform::getDataFolder(fileName);
					std::vector<std::string> contents = CrossPlatform::getFolderContents(folder.str());
					for (std::vector<std::string>::iterator k = contents.begin();
						k != contents.end(); ++k)
					{
						try
						{
							s.str("");
							s << folder.str() << CrossPlatform::getDataFile(*k);
							if (_sets[sheetName]->getFrame(offset))
							{
								if (debugOutput)
								{
									Log(LOG_INFO) << "Replacing frame: " << offset;
								}
								_sets[sheetName]->getFrame(offset)->loadImage(s.str());
							}
							else
							{
								if (adding)
								{
									_sets[sheetName]->addFrame(offset)->loadImage(s.str());
								}
								else
								{
									if (debugOutput)
									{
										Log(LOG_INFO) << "Adding frame: " << offset + spritePack->getModIndex();
									}
									_sets[sheetName]->addFrame(offset + spritePack->getModIndex())->loadImage(s.str());
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
						s << CrossPlatform::getDataFile(fileName);
						if (_sets[sheetName]->getFrame(startFrame))
						{
							if (debugOutput)
							{
								Log(LOG_INFO) << "Replacing frame: " << startFrame;
							}
							_sets[sheetName]->getFrame(startFrame)->loadImage(s.str());
						}
						else
						{
							if (debugOutput)
							{
								Log(LOG_INFO) << "Adding frame: " << startFrame << ", using index: " << startFrame + spritePack->getModIndex();
							}
							_sets[sheetName]->addFrame(startFrame + spritePack->getModIndex())->loadImage(s.str());
						}
					}
					else
					{
						Surface *temp = new Surface(spritePack->getWidth(), spritePack->getHeight());
						s.str("");
						s << CrossPlatform::getDataFile(spritePack->getSprites()->operator[](startFrame));
						temp->loadImage(s.str());
						int xDivision = spritePack->getWidth() / spritePack->getSubX();
						int yDivision = spritePack->getHeight() / spritePack->getSubY();
						int offset = startFrame;

						for (int y = 0; y != yDivision; ++y)
						{
							for (int x = 0; x != xDivision; ++x)
							{
								if (_sets[sheetName]->getFrame(offset))
								{
									if (debugOutput)
									{
										Log(LOG_INFO) << "Replacing frame: " << offset;
									}
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
										if (debugOutput)
										{
											Log(LOG_INFO) << "Adding frame: " << offset + spritePack->getModIndex();
										}
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

	for (std::vector<std::pair<std::string, ExtraSounds *> >::const_iterator i = extraSounds.begin(); i != extraSounds.end(); ++i)
	{
		std::string setName = i->first;
		ExtraSounds *soundPack = i->second;
		if (_sounds.find(setName) == _sounds.end())
		{
			if (debugOutput)
			{
				Log(LOG_INFO) << "Creating new sound set: " << setName << ", this will likely have no in-game use.";
			}
			_sounds[setName] = new SoundSet();
		}
		else if (debugOutput)
		{
			Log(LOG_INFO) << "Adding/Replacing items in sound set: " << setName;
		}
		for (std::map<int, std::string>::iterator j = soundPack->getSounds()->begin(); j != soundPack->getSounds()->end(); ++j)
		{
			int startSound = j->first;
			std::string fileName = j->second;
			s.str("");
			if (fileName.substr(fileName.length() - 1, 1) == "/")
			{
				if (debugOutput)
				{
					Log(LOG_INFO) << "Loading sound set from folder: " << fileName << " starting at index: " << startSound;
				}
				int offset = startSound;
				std::stringstream folder;
				folder << CrossPlatform::getDataFolder(fileName);
				std::vector<std::string> contents = CrossPlatform::getFolderContents(folder.str());
				for (std::vector<std::string>::iterator k = contents.begin();
					k != contents.end(); ++k)
				{
					try
					{
						s.str("");
						s << folder.str() << CrossPlatform::getDataFile(*k);
						if (_sounds[setName]->getSound(offset))
						{
							_sounds[setName]->getSound(offset)->load(s.str());
						}
						else
						{
							_sounds[setName]->addSound(offset + soundPack->getModIndex())->load(s.str());
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
				s << CrossPlatform::getDataFile(fileName);
				if (_sounds[setName]->getSound(startSound))
				{
					if (debugOutput)
					{
						Log(LOG_INFO) << "Replacing index: " << startSound;
					}
					_sounds[setName]->getSound(startSound)->load(s.str());
				}
				else
				{
					if (debugOutput)
					{
						Log(LOG_INFO) << "Adding index: " << startSound;
					}
					_sounds[setName]->addSound(startSound + soundPack->getModIndex())->load(s.str());
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


void XcomResourcePack::loadBattlescapeResources()
{
	// Load Battlescape ICONS
	std::stringstream s;
	s << "UFOGRAPH/" << "SPICONS.DAT";
	_sets["SPICONS.DAT"] = new SurfaceSet(32, 24);
	_sets["SPICONS.DAT"]->loadDat(CrossPlatform::getDataFile(s.str()));

	s.str("");
	std::stringstream s2;
	s << "UFOGRAPH/" << "CURSOR.PCK";
	s2 << "UFOGRAPH/" << "CURSOR.TAB";
	_sets["CURSOR.PCK"] = new SurfaceSet(32, 40);
	_sets["CURSOR.PCK"]->loadPck(CrossPlatform::getDataFile(s.str()), CrossPlatform::getDataFile(s2.str()));

	s.str("");
	s2.str("");
	s << "UFOGRAPH/" << "SMOKE.PCK";
	s2 << "UFOGRAPH/" << "SMOKE.TAB";
	_sets["SMOKE.PCK"] = new SurfaceSet(32, 40);
	_sets["SMOKE.PCK"]->loadPck(CrossPlatform::getDataFile(s.str()), CrossPlatform::getDataFile(s2.str()));
	
	s.str("");
	s2.str("");
	s << "UFOGRAPH/" << "HIT.PCK";
	s2 << "UFOGRAPH/" << "HIT.TAB";
	_sets["HIT.PCK"] = new SurfaceSet(32, 40);
	_sets["HIT.PCK"]->loadPck(CrossPlatform::getDataFile(s.str()), CrossPlatform::getDataFile(s2.str()));

	s.str("");
	s2.str("");
	s << "UFOGRAPH/" << "X1.PCK";
	s2 << "UFOGRAPH/" << "X1.TAB";
	_sets["X1.PCK"] = new SurfaceSet(128, 64);
	_sets["X1.PCK"]->loadPck(CrossPlatform::getDataFile(s.str()), CrossPlatform::getDataFile(s2.str()));

	s.str("");
	_sets["MEDIBITS.DAT"] = new SurfaceSet(52, 58);
	s << "UFOGRAPH/" << "MEDIBITS.DAT";
	_sets["MEDIBITS.DAT"]->loadDat (CrossPlatform::getDataFile(s.str()));

	s.str("");
	_sets["DETBLOB.DAT"] = new SurfaceSet(16, 16);
	s << "UFOGRAPH/" << "DETBLOB.DAT";
	_sets["DETBLOB.DAT"]->loadDat (CrossPlatform::getDataFile(s.str()));

	// Load Battlescape Terrain (only blacks are loaded, others are loaded just in time)
	std::string bsets[] = {"BLANKS.PCK"};

	for (int i = 0; i < 1; ++i)
	{
		std::stringstream s;
		s << "TERRAIN/" << bsets[i];
		std::string tab = CrossPlatform::noExt(bsets[i]) + ".TAB";
		std::stringstream s2;
		s2 << "TERRAIN/" << tab;
		_sets[bsets[i]] = new SurfaceSet(32, 40);
		_sets[bsets[i]]->loadPck(CrossPlatform::getDataFile(s.str()), CrossPlatform::getDataFile(s2.str()));
	}

	// Load Battlescape units
	std::string units = CrossPlatform::getDataFolder("UNITS/");
	std::vector<std::string> usets = CrossPlatform::getFolderContents(units, "PCK");
	for (std::vector<std::string>::iterator i = usets.begin(); i != usets.end(); ++i)
	{
		std::string path = units + *i;
		std::string tab = CrossPlatform::getDataFile("UNITS/" + CrossPlatform::noExt(*i) + ".TAB");
		std::transform(i->begin(), i->end(), i->begin(), toupper);
		if (*i != "BIGOBS.PCK")
			_sets[*i] = new SurfaceSet(32, 40);
		else
			_sets[*i] = new SurfaceSet(32, 48);
		_sets[*i]->loadPck(path, tab);
	}

	s.str("");
	s << "GEODATA/" << "LOFTEMPS.DAT";
	MapDataSet::loadLOFTEMPS(CrossPlatform::getDataFile(s.str()), &_voxelData);

	std::string scrs[] = {"TAC00.SCR"};

	for (int i = 0; i < 1; ++i)
	{
		std::stringstream s;
		s << "UFOGRAPH/" << scrs[i];
		_surfaces[scrs[i]] = new Surface(320, 200);
		_surfaces[scrs[i]]->loadScr(CrossPlatform::getDataFile(s.str()));
	}

	std::string spks[] = {"TAC01.SCR",
						  "DETBORD.PCK",
						  "DETBORD2.PCK",
						  "ICONS.PCK",
						  "MEDIBORD.PCK",
						  "SCANBORD.PCK",
						  "UNIBORD.PCK"};

	for (int i = 0; i < 7; ++i)
	{
		std::stringstream s;
		s << "UFOGRAPH/" << spks[i];
		_surfaces[spks[i]] = new Surface(320, 200);
		_surfaces[spks[i]]->loadSpk(CrossPlatform::getDataFile(s.str()));
	}

	// Load Battlescape inventory
	std::string ufograph = CrossPlatform::getDataFolder("UFOGRAPH/");
	std::vector<std::string> invs = CrossPlatform::getFolderContents(ufograph, "SPK");
	for (std::vector<std::string>::iterator i = invs.begin(); i != invs.end(); ++i)
	{
		std::string path = ufograph + *i;
		std::transform(i->begin(), i->end(), i->begin(), toupper);
		_surfaces[*i] = new Surface(320, 200);
		_surfaces[*i]->loadSpk(path);
	}

	//"fix" of hair color of male personal armor
	if (Options::getBool("battleHairBleach"))
	{
		SurfaceSet *xcom_1 = _sets["XCOM_1.PCK"];

		for (int i = 0; i < 16; ++i)
		{
			//chest frame
			Surface *surf = xcom_1->getFrame(4 * 8 + i);
			ShaderMove<Uint8> head = ShaderMove<Uint8>(surf);
			GraphSubset dim = head.getBaseDomain();
			surf->lock();
			dim.beg_y = 6;
			dim.end_y = 9;
			head.setDomain(dim);
			ShaderDraw<HairBleach>(head, ShaderScalar<Uint8>(HairBleach::Face + 5));
			dim.beg_y = 9;
			dim.end_y = 10;
			head.setDomain(dim);
			ShaderDraw<HairBleach>(head, ShaderScalar<Uint8>(HairBleach::Face + 6));
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
			ShaderDraw<HairBleach>(head, ShaderScalar<Uint8>(HairBleach::Face + 6));
			surf->unlock();
		}
	}
}

}
