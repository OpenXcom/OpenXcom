#pragma once
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
#include "OptionsBaseState.h"

namespace OpenXcom
{

class Text;
class ComboBox;
class Slider;
class ToggleTextButton;

/**
 * Screen that lets the user configure various
 * Audio options.
 */
class OptionsAudioState : public OptionsBaseState
{
private:
	static const std::string musFormats[], sndFormats[];
	Text *_txtMusicVolume, *_txtSoundVolume, *_txtUiVolume;
	Slider *_slrMusicVolume, *_slrSoundVolume, *_slrUiVolume;
	Text *_txtMusicFormat, *_txtCurrentMusic, *_txtSoundFormat, *_txtCurrentSound, *_txtVideoFormat;
	ComboBox *_cbxMusicFormat, *_cbxSoundFormat, *_cbxVideoFormat;
	Text* _txtOptions;
	ToggleTextButton *_btnBackgroundMute;
public:
	/// Creates the Audio Options state.
	OptionsAudioState(OptionsOrigin origin);
	/// Cleans up the Audio Options state.
	~OptionsAudioState();
	/// Handler for changing the music slider.
	void slrMusicVolumeChange(Action *action);
	/// Handler for changing the sound slider.
	void slrSoundVolumeChange(Action *action);
	/// Handler for sound slider button release.
	void slrSoundVolumeRelease(Action *action);
	/// Handler for changing the sound slider.
	void slrUiVolumeChange(Action *action);
	/// Handler for sound slider button release.
	void slrUiVolumeRelease(Action *action);
	/// Handler for changing the Music Format combobox.
	void cbxMusicFormatChange(Action *action);
	/// Handler for changing the Sound Format combobox.
	void cbxSoundFormatChange(Action *action);
	/// Handler for changing the Video Format combobox.
	void cbxVideoFormatChange(Action *action);
	/// Handler for clicking the Background Mute button.
	void btnBackgroundMuteClick(Action* action);
};

}
