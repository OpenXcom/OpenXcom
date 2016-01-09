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
#include "OptionsNoAudioState.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/Text.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Audio Options screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
OptionsNoAudioState::OptionsNoAudioState(OptionsOrigin origin) : OptionsBaseState(origin)
{
	setCategory(_btnAudio);

	// Create objects
	_txtError = new Text(218, 136, 94, 8);

	add(_txtError, "text", "audioMenu");

	centerAllSurfaces();

	// Set up objects
	_txtError->setAlign(ALIGN_CENTER);
	_txtError->setVerticalAlign(ALIGN_MIDDLE);
	_txtError->setBig();
	_txtError->setWordWrap(true);
	_txtError->setText(tr("STR_NO_AUDIO_HARDWARE_DETECTED"));
}

/**
 *
 */
OptionsNoAudioState::~OptionsNoAudioState()
{

}

}
