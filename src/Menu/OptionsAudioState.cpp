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
#include "OptionsAudioState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/ComboBox.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/Slider.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"
#include "../Engine/Sound.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Audio Options screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
OptionsAudioState::OptionsAudioState(Game *game, OptionsOrigin origin) : OptionsBaseState(game, origin)
{
	setCategory(_btnAudio);

	// Create objects
	_txtMusicVolume = new Text(114, 9, 94, 8);
	_slrMusicVolume = new Slider(100, 16, 94, 18);

	_txtSoundVolume = new Text(114, 9, 210, 8);
	_slrSoundVolume = new Slider(100, 16, 210, 18);

	_txtUiVolume = new Text(114, 9, 94, 40);
	_slrUiVolume = new Slider(100, 16, 94, 50);

	_txtBitDepth = new Text(114, 9, 94, 72);
	_cbxBitDepth = new ComboBox(this, 100, 16, 94, 82);

	_txtSampleRate = new Text(114, 9, 210, 72);
	_cbxSampleRate = new ComboBox(this, 100, 16, 210, 82);

	add(_txtMusicVolume);
	add(_slrMusicVolume);

	add(_txtSoundVolume);
	add(_slrSoundVolume);

	add(_txtUiVolume);
	add(_slrUiVolume);

	add(_txtBitDepth);
	add(_txtSampleRate);

	add(_cbxSampleRate);
	add(_cbxBitDepth);

	centerAllSurfaces();

	// Set up objects
	_txtMusicVolume->setColor(Palette::blockOffset(8)+10);
	_txtMusicVolume->setText(tr("STR_MUSIC_VOLUME"));

	_slrMusicVolume->setColor(Palette::blockOffset(15)-1);
	_slrMusicVolume->setRange(0, SDL_MIX_MAXVOLUME);
	_slrMusicVolume->setValue(Options::musicVolume);
	_slrMusicVolume->onChange((ActionHandler)&OptionsAudioState::slrMusicVolumeChange);
	_slrMusicVolume->setTooltip("STR_MUSIC_VOLUME_DESC");
	_slrMusicVolume->onMouseIn((ActionHandler)&OptionsAudioState::txtTooltipIn);
	_slrMusicVolume->onMouseOut((ActionHandler)&OptionsAudioState::txtTooltipOut);

	_txtSoundVolume->setColor(Palette::blockOffset(8)+10);
	_txtSoundVolume->setText(tr("STR_SFX_VOLUME"));

	_slrSoundVolume->setColor(Palette::blockOffset(15)-1);
	_slrSoundVolume->setRange(0, SDL_MIX_MAXVOLUME);
	_slrSoundVolume->setValue(Options::soundVolume);
	_slrSoundVolume->onChange((ActionHandler)&OptionsAudioState::slrSoundVolumeChange);
	_slrSoundVolume->onMouseRelease((ActionHandler)&OptionsAudioState::slrSoundVolumeRelease);
	_slrSoundVolume->setTooltip("STR_SFX_VOLUME_DESC");
	_slrSoundVolume->onMouseIn((ActionHandler)&OptionsAudioState::txtTooltipIn);
	_slrSoundVolume->onMouseOut((ActionHandler)&OptionsAudioState::txtTooltipOut);

	_txtUiVolume->setColor(Palette::blockOffset(8)+10);
	_txtUiVolume->setText(tr("STR_UI_VOLUME"));

	_slrUiVolume->setColor(Palette::blockOffset(15)-1);
	_slrUiVolume->setRange(0, SDL_MIX_MAXVOLUME);
	_slrUiVolume->setValue(Options::uiVolume);
	_slrUiVolume->onChange((ActionHandler)&OptionsAudioState::slrUiVolumeChange);
	_slrUiVolume->onMouseRelease((ActionHandler)&OptionsAudioState::slrUiVolumeRelease);
	_slrUiVolume->setTooltip("STR_UI_VOLUME_DESC");
	_slrUiVolume->onMouseIn((ActionHandler)&OptionsAudioState::txtTooltipIn);
	_slrUiVolume->onMouseOut((ActionHandler)&OptionsAudioState::txtTooltipOut);

	_txtBitDepth->setColor(Palette::blockOffset(8)+10);
	_txtBitDepth->setText(tr("STR_AUDIO_BIT_DEPTH"));

	std::wostringstream ss;
	std::vector<std::wstring> bitsText, samplesText;
	int bits[] = {8, 16};
	for (int i = 0; i < sizeof(bits) / sizeof(bits[0]); ++i)
	{
		_bitDepths.push_back(bits[i]);
		ss << bits[i] << L"-bit";
		bitsText.push_back(ss.str());
		ss.str(L"");
		if (Options::audioBitDepth == bits[i])
		{
			_cbxBitDepth->setSelected(i);
		}
	}
	int samples[] = {8000, 11025, 16000, 22050, 32000, 44100, 48000};
	for (int i = 0; i < sizeof(samples) / sizeof(samples[0]); ++i)
	{
		_sampleRates.push_back(samples[i]);
		ss << samples[i] << L" Hz";
		samplesText.push_back(ss.str());
		ss.str(L"");
		if (Options::audioSampleRate == samples[i])
		{
			_cbxSampleRate->setSelected(i);
		}
	}

	std::vector<std::wstring> bitLabels, bitSamples;

	_cbxBitDepth->setColor(Palette::blockOffset(15)-1);
	_cbxBitDepth->setOptions(bitsText);
	_cbxBitDepth->setTooltip("STR_AUDIO_BIT_DEPTH_DESC");
	_cbxBitDepth->onChange((ActionHandler)&OptionsAudioState::cbxBitDepthChange);
	_cbxBitDepth->onMouseIn((ActionHandler)&OptionsAudioState::txtTooltipIn);
	_cbxBitDepth->onMouseOut((ActionHandler)&OptionsAudioState::txtTooltipOut);

	_txtSampleRate->setColor(Palette::blockOffset(8)+10);
	_txtSampleRate->setText(tr("STR_AUDIO_SAMPLE_RATE"));

	_cbxSampleRate->setColor(Palette::blockOffset(15)-1);
	_cbxSampleRate->setOptions(samplesText);
	_cbxSampleRate->setTooltip("STR_AUDIO_SAMPLE_RATE_DESC");
	_cbxSampleRate->onChange((ActionHandler)&OptionsAudioState::cbxSampleRateChange);
	_cbxSampleRate->onMouseIn((ActionHandler)&OptionsAudioState::txtTooltipIn);
	_cbxSampleRate->onMouseOut((ActionHandler)&OptionsAudioState::txtTooltipOut);

	// These options require a restart, so don't enable them in-game
	_txtBitDepth->setVisible(_origin == OPT_MENU);
	_cbxBitDepth->setVisible(_origin == OPT_MENU);
	_txtSampleRate->setVisible(_origin == OPT_MENU);
	_cbxSampleRate->setVisible(_origin == OPT_MENU);
}

/**
 *
 */
OptionsAudioState::~OptionsAudioState()
{

}

/**
 * Updates the music volume.
 * @param action Pointer to an action.
 */
void OptionsAudioState::slrMusicVolumeChange(Action *)
{
	Options::musicVolume = _slrMusicVolume->getValue();
	_game->setVolume(Options::soundVolume, Options::musicVolume, Options::uiVolume);
}

/**
 * Updates the sound volume with the slider.
 * @param action Pointer to an action.
 */
void OptionsAudioState::slrSoundVolumeChange(Action *)
{
	Options::soundVolume = _slrSoundVolume->getValue();
	_game->setVolume(Options::soundVolume, Options::musicVolume, Options::uiVolume);
}

/**
 * Plays a game sound for volume preview.
 * @param action Pointer to an action.
 */
void OptionsAudioState::slrSoundVolumeRelease(Action *)
{
	_game->getResourcePack()->getSound("GEO.CAT", 5)->play();
}

/**
 * Updates the UI volume with the slider.
 * @param action Pointer to an action.
 */
void OptionsAudioState::slrUiVolumeChange(Action *)
{
	Options::uiVolume = _slrUiVolume->getValue();
	_game->setVolume(Options::soundVolume, Options::musicVolume, Options::uiVolume);
}

/**
 * Plays a UI sound for volume preview.
 * @param action Pointer to an action.
 */
void OptionsAudioState::slrUiVolumeRelease(Action *)
{
	_game->getResourcePack()->getSound("GEO.CAT", 0)->play(0);
}

/**
 * Changes the Audio Bit Depth option.
 * @param action Pointer to an action.
 */
void OptionsAudioState::cbxBitDepthChange(Action *)
{
	Options::audioBitDepth = _bitDepths[_cbxBitDepth->getSelected()];
	Options::reload = true;
}

/**
 * Changes the Audio Sample Rate option.
 * @param action Pointer to an action.
 */
void OptionsAudioState::cbxSampleRateChange(Action *)
{
	Options::audioSampleRate = _sampleRates[_cbxSampleRate->getSelected()];
	Options::reload = true;
}

}
