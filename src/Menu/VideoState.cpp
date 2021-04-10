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
#include "VideoState.h"
#include <algorithm>
#include <SDL_mixer.h>
#include "../Engine/Adlib/adlplayer.h"
#include "../Engine/Logger.h"
#include "../Engine/Game.h"
#include "../Engine/Options.h"
#include "../Engine/FlcPlayer.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/FileMap.h"
#include "../Engine/Screen.h"
#include "../Engine/Music.h"
#include "../Engine/Sound.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleVideo.h"
#include "CutsceneState.h"
#include "../Interface/Cursor.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Intro screen.
 * @param game Pointer to the core game.
 * @param wasLetterBoxed Was the game letterboxed?
 */
VideoState::VideoState(const std::vector<std::string> *videos, const std::vector<std::string> *tracks, bool useUfoAudioSequence)
		: _videos(videos), _tracks(tracks), _useUfoAudioSequence(useUfoAudioSequence)
{
}

/**
 *
 */
VideoState::~VideoState()
{
}


typedef struct
{
	std::string catFile;
	int sound;
	int volume;
} soundInFile;


// the pure MS-DOS experience
static soundInFile introCatOnlySounds[]=
{
{"INTRO.CAT", 0x0, 32},
{"INTRO.CAT", 0x1, 32},
{"INTRO.CAT", 0x2, 32},
{"INTRO.CAT", 0x3, 32},
{"INTRO.CAT", 0x4, 32},
{"INTRO.CAT", 0x5, 32},
{"INTRO.CAT", 0x6, 32},
{"INTRO.CAT", 0x7, 32},
{"INTRO.CAT", 0x8, 32},
{"INTRO.CAT", 0x9, 32},
{"INTRO.CAT", 0xa, 32},
{"INTRO.CAT", 0xb, 32},
{"INTRO.CAT", 0xc, 32},
{"INTRO.CAT", 0xd, 32},
{"INTRO.CAT", 0xe, 32},
{"INTRO.CAT", 0xf, 32},
{"INTRO.CAT", 0x10, 32},
{"INTRO.CAT", 0x11, 32},
{"INTRO.CAT", 0x12, 32},
{"INTRO.CAT", 0x13, 32},
{"INTRO.CAT", 0x14, 32},
{"INTRO.CAT", 0x15, 32},
{"INTRO.CAT", 0x16, 32},
{"INTRO.CAT", 0x17, 32},
{"INTRO.CAT", 0x18, 32},
{"INTRO.CAT", 0x18, 32}
};


static soundInFile sample3CatOnlySounds[]=
{
{"SAMPLE3.CAT", 24, 32}, // machine gun
{"SAMPLE3.CAT", 5, 32},   // plasma rifle
{"SAMPLE3.CAT", 23, 32}, // rifle
{"SAMPLE3.CAT",  6, 32}, // some kind of death noise, urgh?
{"SAMPLE3.CAT", 9, 64}, // mutdie
{"SAMPLE3.CAT", 7, 64}, // dying alien
{"SAMPLE3.CAT", 27, 64}, // another dying alien
{"SAMPLE3.CAT", 4, 32}, // ??? ship flying? alien screech?
{"SAMPLE3.CAT", 0x8, 32}, // fscream
{"SAMPLE3.CAT", 11, 32}, // alarm
{"SAMPLE3.CAT", 4, 32}, // gun spinning up?
{"INTRO.CAT", 0xb, 32},  // reload; this one's not even in sample3
{"SAMPLE3.CAT",19, 48},  // whoosh
{"INTRO.CAT", 0xd, 32},  // feet, also not in sample3
{"SAMPLE3.CAT", 2, 32},  // low pulsating hum
{"SAMPLE3.CAT", 30, 32}, // energise
{"SAMPLE3.CAT", 21, 32}, // hatch
{"SAMPLE3.CAT", 0, 64}, // phizz -- no equivalent in sample3.cat?
{"SAMPLE3.CAT", 13, 32}, // warning
{"SAMPLE3.CAT", 14, 32}, // detected
{"SAMPLE3.CAT", 19, 64}, // UFO flyby whoosh?
{"SAMPLE3.CAT", 3, 32}, // growl
{"SAMPLE3.CAT", 15, 128}, // voice
{"SAMPLE3.CAT", 12, 32}, // beep 1
{"SAMPLE3.CAT", 18, 32}, // takeoff
{"SAMPLE3.CAT", 20, 32}  // another takeoff/landing sound?? if it exists?
};



// an attempt at a mix of (subjectively) the best sounds from the two versions
// difficult because we can't find a definitive map from old sequence numbers to SAMPLE3.CAT indexes
// probably only the Steam version of the game comes with both INTRO.CAT and SAMPLE3.CAT
static soundInFile hybridIntroSounds[]=
{
{"SAMPLE3.CAT", 24, 32}, // machine gun
{"SAMPLE3.CAT", 5, 32},   // plasma rifle
{"SAMPLE3.CAT", 23, 32}, // rifle
{"INTRO.CAT",  3, 32}, // some kind of death noise, urgh?
{"INTRO.CAT", 0x4, 64}, // mutdie
{"INTRO.CAT", 0x5, 64}, // dying alien
{"INTRO.CAT", 0x6, 64}, // another dying alien
{"INTRO.CAT", 0x7, 32}, // ??? ship flying? alien screech?
{"SAMPLE3.CAT", 0x8, 32}, // fscream
{"SAMPLE3.CAT", 11, 32}, // alarm
{"SAMPLE3.CAT", 4, 32}, // gun spinning up?
{"INTRO.CAT", 0xb, 32},  // reload; this one's not even in sample3
{"SAMPLE3.CAT",19, 48},  // whoosh
{"INTRO.CAT", 0xd, 32},  // feet, also not in sample3
{"INTRO.CAT", 0xe, 32},  // low pulsating hum
{"SAMPLE3.CAT", 30, 32}, // energise
{"SAMPLE3.CAT", 21, 32}, // hatch
{"INTRO.CAT", 0x11, 64}, // phizz
{"SAMPLE3.CAT", 13, 32}, // warning
{"SAMPLE3.CAT", 14, 32}, // detected
{"SAMPLE3.CAT", 19, 64}, // UFO flyby whoosh?
{"INTRO.CAT", 0x15, 32}, // growl
{"SAMPLE3.CAT", 15, 128}, // voice
{"SAMPLE3.CAT", 12, 32}, // beep 1
{"SAMPLE3.CAT", 18, 32}, // takeoff
{"SAMPLE3.CAT", 20, 32}  // another takeoff/landing sound?? if it exists?
};

// sample3: 18 is takeoff, 20 is landing; 19 is flyby whoosh sound, not sure for which craft

static soundInFile *introSounds[] =
{
	hybridIntroSounds,
	introCatOnlySounds,
	sample3CatOnlySounds,
	0
};


typedef struct
{
	int frameNumber;
	int sound;
} introSoundEffect;

static introSoundEffect introSoundTrack[] =
{
{0, 0x200}, // inserting this to keep the code simple
{149, 0x11},
{173, 0x0C},
{183, 0x0E},
{205, 0x15},
{211, 0x201},
{211, 0x407},
{223, 0x7},
{250, 0x1},
{253, 0x1},
{255, 0x1},
{257, 0x1},
{260, 0x1},
{261, 0x3},
{262, 0x1},
{264, 0x1},
{268, 0x1},
{270, 0x1},
{272, 0x5},
{272, 0x1},
{274, 0x1},
{278, 0x1},
{280, 0x1},
{282, 0x8},
{282, 0x1},
{284, 0x1},
{286, 0x1},
{288, 0x1},
{290, 0x1},
{292, 0x6},
{292, 0x1},
{296, 0x1},
{298, 0x1},
{300, 0x1},
{302, 0x1},
{304, 0x1},
{306, 0x1},
{308, 0x1},
{310, 0x1},
{312, 0x1},
{378, 0x202},
{378, 0x9}, // alarm
{386, 0x9},
{393, 0x9},
{399, 0x17}, // bleeps
{433, 0x17},
{463, 0x12}, // warning
{477, 0x12},
{487, 0x13}, // ufo detected
{495, 0x16}, // voice
{501, 0x16},
{512, 0xd},  // feet -- not in original
{514, 0xd},  // feet -- not in original
{522, 0x0B}, // rifle grab
{523, 0xd},  // feet -- not in original
{525, 0xd},  // feet -- not in original
{534, 0x18},
{535, 0x405},
{560, 0x407},
{577, 0x14},
{582, 0x405},
// {582, 0x18}, // landing! correcting to landing sound!
{582, 0x19},
{613, 0x407},
{615, 0x10},
{635, 0x14},
{638, 0x14},
{639, 0x14},
{644, 0x2},
{646, 0x2},
{648, 0x2},
{650, 0x2},
{652, 0x2},
{654, 0x2},
{656, 0x2},
{658, 0x2},
{660, 0x2},
{662, 0x2},
{664, 0x2},
{666, 0x2},
{668, 0x401},
{681, 0x406},
{687, 0x402},
{689, 0x407},
{694, 0x0A},
{711, 0x407},
{711, 0x0},
{714, 0x0},
{716, 0x4},
{717, 0x0},
{720, 0x0},
{723, 0x0},
{726, 0x5},
{726, 0x0},
{729, 0x0},
{732, 0x0},
{735, 0x0},
{738, 0x0},
{741, 0x0},
{742, 0x6},
{744, 0x0},
{747, 0x0},
{750, 0x0},
{753, 0x0},
{756, 0x0},
{759, 0x0},
{762, 0x0},
{765, 0x0},
{768, 0x0},
{771, 0x0},
{774, 0x0},
{777, 0x0},
{780, 0x0},
{783, 0x0},
{786, 0x0},
{790, 0x15},
{790, 0x15},
{807, 0x2},
{810, 0x2},
{812, 0x2},
{814, 0x2},
{816, 0x0},
{819, 0x0},
{822, 0x0},
{824, 0x40A},
{824, 0x5},
{827, 0x6},
{835, 0x0F},
{841, 0x0F},
{845, 0x0F},
{855, 0x407},
{879, 0x0C},
{65535, 0x0FFFF}
};


static struct AudioSequence
{
	Mod *mod;
	Music *m;
	Sound *s;
	int trackPosition;
	FlcPlayer *_flcPlayer;

	AudioSequence(Mod *_mod, FlcPlayer *flcPlayer) : mod(_mod), m(0), s(0), trackPosition(0), _flcPlayer(flcPlayer)
	{ }

	void operator()()
	{
		while (_flcPlayer->getFrameCount() >= introSoundTrack[trackPosition].frameNumber)
		{
			int command = introSoundTrack[trackPosition].sound;

			if (command & 0x200)
			{
#ifndef __NO_MUSIC
				switch(command)
				{
				case 0x200:
					Log(LOG_DEBUG) << "Playing gmintro1";
					m = mod->getMusic("GMINTRO1");
					m->play(1);
					break;
				case 0x201:
					Log(LOG_DEBUG) << "Playing gmintro2";
					m = mod->getMusic("GMINTRO2");
					m->play(1);
					break;
				case 0x202:
					Log(LOG_DEBUG) << "Playing gmintro3";
					m = mod->getMusic("GMINTRO3");
					m->play(1);
					//Mix_HookMusicFinished(_FlcPlayer::stop);
					break;
				}
#endif
			}
			else if (command & 0x400)
			{
				int newSpeed = (command & 0xff);
				_flcPlayer->setHeaderSpeed(newSpeed);
				Log(LOG_DEBUG) << "Frame delay now: " << newSpeed;
			}
			else if (command <= 0x19)
			{
				for (soundInFile **sounds = introSounds; *sounds; ++sounds) // try hybrid sound set, then intro.cat or sample3.cat alone
				{
					soundInFile *sf = (*sounds) + command;
					int channel = trackPosition % 4; // use at most four channels to play sound effects
					double ratio = (double)Options::soundVolume / MIX_MAX_VOLUME;
					Log(LOG_DEBUG) << "playing: " << sf->catFile << ":" << sf->sound << " for index " << command;
					s = mod->getSound(sf->catFile, sf->sound, false);
					if (s)
					{
						s->play(channel);
						Mix_Volume(channel, sf->volume * ratio);
						break;
					}
					else Log(LOG_DEBUG) << "Couldn't play " << sf->catFile << ":" << sf->sound;
				}
			}
			++trackPosition;
		}

	}
} *audioSequence = NULL;


static void audioHandler()
{
	(*audioSequence)();
}

void VideoState::init()
{
	State::init();

	bool wasLetterboxed = CutsceneState::initDisplay();

	bool ufoIntroSoundFileDosExists = false;
	bool ufoIntroSoundFileWinExists = false;
	int prevMusicVol = Options::musicVolume;
	int prevSoundVol = Options::soundVolume;
	if (_useUfoAudioSequence)
	{
		const std::set<std::string> &soundDir = FileMap::getVFolderContents("SOUND");
		ufoIntroSoundFileDosExists = soundDir.end() != soundDir.find("intro.cat");
		ufoIntroSoundFileWinExists = soundDir.end() != soundDir.find("sample3.cat");

		if (!ufoIntroSoundFileDosExists && !ufoIntroSoundFileWinExists)
		{
			_useUfoAudioSequence = false;
		}
		else
		{
			// ensure user can hear both music and sound effects for the
			// vanilla intro sequence
			Options::musicVolume = Options::soundVolume = std::max(prevMusicVol, prevSoundVol);
			_game->setVolume(Options::soundVolume, Options::musicVolume, -1);
		}
	}
	_game->getCursor()->setVisible(false);

	int dx = (Options::baseXResolution - Screen::ORIGINAL_WIDTH) / 2;
	int dy = (Options::baseYResolution - Screen::ORIGINAL_HEIGHT) / 2;

	// We can only do a fade out in 8bpp, otherwise instantly end it
	bool fade = (_game->getScreen()->getSurface()->getSurface()->format->BitsPerPixel == 8);
	const int FADE_DELAY = 45;
	const int FADE_STEPS = 20;

	FlcPlayer *flcPlayer = NULL;
	size_t audioCounter = 0;
	for (std::vector<std::string>::const_iterator it = _videos->begin(); it != _videos->end(); ++it)
	{
		bool useInternalAudio = true;
		if (!_tracks->empty() && _tracks->size() > audioCounter && _game->getMod()->getMusic(_tracks->at(audioCounter)))
		{
			_game->getMod()->getMusic(_tracks->at(audioCounter))->play(0);
			useInternalAudio = false;
		}
		audioCounter++;
		const std::string& videoFileName = FileMap::getFilePath(*it);

		if (!CrossPlatform::fileExists(videoFileName))
		{
			continue;
		}

		if (!flcPlayer)
		{
			flcPlayer = new FlcPlayer();
		}

		if (_useUfoAudioSequence)
		{
			audioSequence = new AudioSequence(_game->getMod(), flcPlayer);
		}

		flcPlayer->init(videoFileName.c_str(),
			 _useUfoAudioSequence ? &audioHandler : NULL,
			 _game, useInternalAudio, dx, dy);
		flcPlayer->play(_useUfoAudioSequence);
		if (_useUfoAudioSequence)
		{
			flcPlayer->delay(10000);
			delete audioSequence;
			audioSequence = NULL;
		}
		flcPlayer->deInit();

		if (flcPlayer->wasSkipped())
		{
			fade = false;
			break;
		}
	}

	if (flcPlayer)
	{
		delete flcPlayer;
	}

#ifndef __NO_MUSIC
	// fade out!
	if (fade)
	{
		Mix_FadeOutChannel(-1, FADE_DELAY * FADE_STEPS);
		// SDL_Mixer has trouble with native midi and volume on windows,
		// which is the most likely use case, so f@%# it.
		if (Mix_GetMusicType(0) != MUS_MID)
		{
			Mix_FadeOutMusic(FADE_DELAY * FADE_STEPS);
			func_fade();
		}
		else
		{
			Mix_HaltMusic();
		}
	}
	else
	{
		Mix_HaltChannel(-1);
		Mix_HaltMusic();
	}
#endif

	if (fade)
	{
		SDL_Color pal[256];
		SDL_Color pal2[256];
		memcpy(pal, _game->getScreen()->getPalette(), sizeof(SDL_Color) * 256);
		for (int i = FADE_STEPS; i > 0; --i)
		{
			SDL_Event event;
			if (SDL_PollEvent(&event) && event.type == SDL_KEYDOWN) break;
			for (int color = 0; color < 256; ++color)
			{
				pal2[color].r = (((int)pal[color].r) * i) / 20;
				pal2[color].g = (((int)pal[color].g) * i) / 20;
				pal2[color].b = (((int)pal[color].b) * i) / 20;
				pal2[color].a = pal[color].a;
			}
			_game->getScreen()->setPalette(pal2, 0, 256, true);
			_game->getScreen()->flip();
			SDL_Delay(FADE_DELAY);
		}
	}
	_game->getScreen()->clear();
	_game->getScreen()->flip();

	if (_useUfoAudioSequence)
	{
		Options::musicVolume = prevMusicVol;
		Options::soundVolume = prevSoundVol;
		_game->setVolume(Options::soundVolume, Options::musicVolume, Options::uiVolume);
	}

#ifndef __NO_MUSIC
	Sound::stop();
	Music::stop();
#endif

	_game->getCursor()->setVisible(true);
	CutsceneState::resetDisplay(wasLetterboxed);
	_game->popState();
}

}
