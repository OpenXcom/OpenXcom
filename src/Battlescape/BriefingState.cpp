/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "BriefingState.h"
#include "BattlescapeState.h"
#include "AliensCrashState.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Music.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/Window.h"
#include "InventoryState.h"
#include "NextTurnState.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/Ruleset.h"
#include "../RuleSet/RuleMissionSequence.h"
#include "../Savegame/Base.h"
#include "../Savegame/Craft.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Ufo.h"
#include <sstream>
#include "../Engine/Options.h"
#include "../Engine/Screen.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Briefing screen.
 * @param game Pointer to the core game.
 * @param craft Pointer to the craft in the mission.
 * @param base Pointer to the base in the mission.
 */
BriefingState::BriefingState(Craft *craft, Base *base)
{
	_screen = true;
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(120, 18, 100, 164);
	_txtTitle = new Text(300, 17, 16, 24);
	_txtTarget = new Text(300, 17, 16, 40);
	_txtCraft = new Text(300, 17, 16, 56);
	_txtBriefing = new Text(274, 64, 16, 72);

	std::string mission = _game->getSavedGame()->getSavedBattle()->getMissionType();
	std::string missionSequenceId = _game->getSavedGame()->getSavedBattle()->getMissionSequenceType();

	std::string background;
	std::string titleText;
	std::string mainText;

	// Set palette
	if (missionSequenceId != "")
	{
		// Use screen values from the mission sequence data	
		RuleMissionSequence *missionSequence = _game->getRuleset()->getMissionSequence(missionSequenceId);
		MissionData missionData = missionSequence->getMission(_game->getSavedGame()->getSavedBattle()->getMissionNumber());

		setPalette("PAL_GEOSCAPE", missionData.briefingScreen.palette);

		_game->getResourcePack()->playMusic(missionData.briefingScreen.music_id);
		background = missionData.briefingScreen.background_id;

		if (_game->getSavedGame()->getSavedBattle()->getMissionNumber() > 0)
		{
			_txtCraft->setVisible(false);
		}

		titleText = missionData.briefingScreen.title_id;
		mainText = missionData.briefingScreen.mainText_id;

		_txtCraft->setY(40);
		_txtBriefing->setY(56);
		_txtTarget->setVisible(false);
	}
	else
	{
		// Use hardcoded values for mission type
		if (mission == "STR_TERROR_MISSION" || mission == "STR_BASE_DEFENSE")
		{
			setPalette("PAL_GEOSCAPE", 2);
			_game->getResourcePack()->playMusic("GMENBASE");
			background = "BACK16.SCR";
		}
		else if (mission == "STR_ALIEN_BASE_ASSAULT")
		{
			setPalette("PAL_GEOSCAPE", 0);
			background = "BACK01.SCR";

			_txtCraft->setY(40);
			_txtBriefing->setY(56);
			_txtTarget->setVisible(false);
		}
		else
		{
			setPalette("PAL_GEOSCAPE", 0);
			_game->getResourcePack()->playMusic("GMDEFEND");
			background = "BACK16.SCR";
		}
		titleText = mission;
		std::ostringstream briefingtext;
		briefingtext << mission.c_str() << "_BRIEFING";
		mainText = briefingtext.str();
	}

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtTarget);

	add(_txtCraft);
	add(_txtBriefing);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BriefingState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&BriefingState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&BriefingState::btnOkClick, Options::keyCancel);

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setBig();

	_txtTarget->setColor(Palette::blockOffset(8)+5);
	_txtTarget->setBig();

	_txtCraft->setColor(Palette::blockOffset(8)+5);
	_txtCraft->setBig();
	std::wstring s;
	if (craft)
	{
		if (craft->getDestination())
		{
			_txtTarget->setText(craft->getDestination()->getName(_game->getLanguage()));
		}

		s = tr("STR_CRAFT_").arg(craft->getName(_game->getLanguage()));
	}
	else if (base)
	{
		s = tr("STR_BASE_UC_").arg(base->getName());
	}
	_txtCraft->setText(s);

	_txtBriefing->setColor(Palette::blockOffset(8)+5);
	_txtBriefing->setWordWrap(true);

	// Show respective mission briefing
	_window->setBackground(_game->getResourcePack()->getSurface(background));

	_txtTitle->setText(tr(titleText));
	_txtBriefing->setText(tr(mainText));

	if (mission == "STR_BASE_DEFENSE")
	{
		// And make sure the base is unmarked.
		base->setRetaliationTarget(false);
	}
}

/**
 *
 */
BriefingState::~BriefingState()
{

}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void BriefingState::btnOkClick(Action *)
{
	_game->popState();
	Options::baseXResolution = Options::baseXBattlescape;
	Options::baseYResolution = Options::baseYBattlescape;
	_game->getScreen()->resetDisplay(false);
	BattlescapeState *bs = new BattlescapeState;
	int liveAliens = 0, liveSoldiers = 0;
	bs->getBattleGame()->tallyUnits(liveAliens, liveSoldiers, false);
	if (liveAliens > 0)
	{
		_game->pushState(bs);
		_game->getSavedGame()->getSavedBattle()->setBattleState(bs);
		_game->pushState(new NextTurnState(_game->getSavedGame()->getSavedBattle(), bs));
		_game->pushState(new InventoryState(false, bs));
	}
	else
	{
		Options::baseXResolution = Options::baseXGeoscape;
		Options::baseYResolution = Options::baseYGeoscape;
		_game->getScreen()->resetDisplay(false);;
		delete bs;
		_game->pushState(new AliensCrashState);
	}
}

}
