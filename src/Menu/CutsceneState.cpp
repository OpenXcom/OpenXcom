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

#include "CutsceneState.h"
#include "MainMenuState.h"
#include "SlideshowState.h"
#include "VideoState.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Logger.h"
#include "../Engine/Options.h"
#include "../Engine/Screen.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/FileMap.h"
#include "../Mod/Mod.h"

namespace OpenXcom
{

CutsceneState::CutsceneState(const std::string &cutsceneId)
	: _cutsceneId(cutsceneId)
{
	// empty
}

CutsceneState::~CutsceneState()
{
	// empty
}

void CutsceneState::init()
{
	State::init();
	
	// pop self off stack and replace with actual player state
	_game->popState();

	const std::map<std::string, RuleVideo*> *videoMods = _game->getMod()->getVideos();
	std::map<std::string, RuleVideo*>::const_iterator videoRuleIt = videoMods->find(_cutsceneId);

	if (videoRuleIt == videoMods->end())
	{
		Log(LOG_WARNING) << "cutscene definition not found: " << _cutsceneId;
		return;
	}

	if (_cutsceneId == "winGame" || _cutsceneId == "loseGame")
	{
		if (_game->getSavedGame() && _game->getSavedGame()->isIronman()
		    && !_game->getSavedGame()->getName().empty())
		{
			std::string filename = CrossPlatform::sanitizeFilename(
				Language::wstrToFs(_game->getSavedGame()->getName())) + ".sav";
			CrossPlatform::deleteFile(Options::getMasterUserFolder() + filename);
		}
		_game->setSavedGame(0);
		_game->setState(new GoToMainMenuState);
	}

	const RuleVideo *videoRule = videoRuleIt->second;
	bool fmv = false, slide = false;
	if (!videoRule->getVideos()->empty())
	{
		std::string file = FileMap::getFilePath(videoRule->getVideos()->front());
		fmv = CrossPlatform::fileExists(file);
	}
	if (!videoRule->getSlides()->empty())
	{
		std::string file = FileMap::getFilePath(videoRule->getSlides()->front().imagePath);
		slide = CrossPlatform::fileExists(file);
	}

	if (fmv && (!slide || Options::preferredVideo == VIDEO_FMV))
	{
		_game->pushState(new VideoState(videoRule->getVideos(), videoRule->useUfoAudioSequence()));
	}
	else if (slide && (!fmv || Options::preferredVideo == VIDEO_SLIDE))
	{
		_game->pushState(new SlideshowState(videoRule->getSlideshowHeader(), videoRule->getSlides()));
	}
	else
	{
		Log(LOG_WARNING) << "cutscene definition empty: " << _cutsceneId;
	}
}

bool CutsceneState::initDisplay()
{
	bool letterboxed = Options::keepAspectRatio;
	Options::keepAspectRatio = true;
	Options::baseXResolution = Screen::ORIGINAL_WIDTH;
	Options::baseYResolution = Screen::ORIGINAL_HEIGHT;
	_game->getScreen()->resetDisplay(false);
	return letterboxed;
}

void CutsceneState::resetDisplay(bool wasLetterboxed)
{
	Options::keepAspectRatio = wasLetterboxed;
	Screen::updateScale(Options::geoscapeScale, Options::geoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, true);
	_game->getScreen()->resetDisplay(false);
}
}
