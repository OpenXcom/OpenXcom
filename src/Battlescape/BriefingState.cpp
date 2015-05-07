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
#include "../Savegame/Base.h"
#include "../Savegame/Craft.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Ufo.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/AlienDeployment.h"
#include "../Ruleset/RuleUfo.h"
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
	AlienDeployment *deployment = _game->getRuleset()->getDeployment(mission);
	Ufo * ufo = 0;
	if (!deployment && craft)
	{
		ufo = dynamic_cast <Ufo*> (craft->getDestination());
		if (ufo) // landing site or crash site.
		{
			deployment = _game->getRuleset()->getDeployment(ufo->getRules()->getType());
		}
	}

	if (!deployment) // none defined - should never happen, but better safe than sorry i guess.
	{
		setPalette("PAL_GEOSCAPE", 0);
		_game->getResourcePack()->playMusic("GMDEFEND");
		_window->setBackground(_game->getResourcePack()->getSurface("BACK16.SCR"));
	}
	else
	{
		BriefingData data = deployment->getBriefingData();
		setPalette("PAL_GEOSCAPE", data.palette);
		_game->getResourcePack()->playMusic(data.music);
		_window->setBackground(_game->getResourcePack()->getSurface(data.background));
		_txtCraft->setY(56 + data.textOffset);
		_txtBriefing->setY(72 + data.textOffset);
		_txtTarget->setVisible(data.showTarget);
		_txtCraft->setVisible(data.showCraft);
	}

	add(_window, "window", "briefing");
	add(_btnOk, "button", "briefing");
	add(_txtTitle, "text", "briefing");
	add(_txtTarget, "text", "briefing");
	add(_txtCraft, "text", "briefing");
	add(_txtBriefing, "text", "briefing");

	centerAllSurfaces();

	// Set up objects
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BriefingState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&BriefingState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&BriefingState::btnOkClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTarget->setBig();
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

	_txtBriefing->setWordWrap(true);

	_txtTitle->setText(tr(mission));
	std::ostringstream briefingtext;
	briefingtext << mission.c_str() << "_BRIEFING";
	_txtBriefing->setText(tr(briefingtext.str()));

	// if this UFO has a specific briefing, use that instead
	if (ufo && ufo->getRules()->getBriefingString() != "")
	{
		briefingtext.str("");
		briefingtext << ufo->getRules()->getBriefingString();
		_txtBriefing->setText(tr(briefingtext.str()));
	}

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
	bs->getBattleGame()->tallyUnits(liveAliens, liveSoldiers);
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
