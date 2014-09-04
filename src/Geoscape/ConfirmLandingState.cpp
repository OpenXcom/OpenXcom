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
#include "ConfirmLandingState.h"
#include <sstream>
#include "../Engine/RNG.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Target.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/Base.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleMissionSequence.h"
#include "../Savegame/TerrorSite.h"
#include "../Savegame/AlienBase.h"
#include "../Savegame/ScriptedEvent.h"
#include "../Savegame/ScriptedEventLocation.h"
#include "../Battlescape/BriefingState.h"
#include "../Battlescape/BattlescapeGenerator.h"
#include "../Geoscape/GeoscapeState.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Confirm Landing window.
 * @param game Pointer to the core game.
 * @param craft Pointer to the craft to confirm.
 * @param texture Texture of the landing site.
 * @param shade Shade of the landing site.
 */
ConfirmLandingState::ConfirmLandingState(Craft *craft, int texture, int shade) : _craft(craft), _texture(texture), _shade(shade)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 216, 160, 20, 20, POPUP_BOTH);
	_btnYes = new TextButton(80, 20, 40, 150);
	_btnNo = new TextButton(80, 20, 136, 150);
	_txtMessage = new Text(206, 80, 25, 40);
	_txtBegin = new Text(206, 17, 25, 130);

	// Set palette
	setPalette("PAL_GEOSCAPE", 3);

	add(_window);
	add(_btnYes);
	add(_btnNo);
	add(_txtMessage);
	add(_txtBegin);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK15.SCR"));

	_btnYes->setColor(Palette::blockOffset(8)+5);
	_btnYes->setText(tr("STR_YES"));
	_btnYes->onMouseClick((ActionHandler)&ConfirmLandingState::btnYesClick);
	_btnYes->onKeyboardPress((ActionHandler)&ConfirmLandingState::btnYesClick, Options::keyOk);

	_btnNo->setColor(Palette::blockOffset(8)+5);
	_btnNo->setText(tr("STR_NO"));
	_btnNo->onMouseClick((ActionHandler)&ConfirmLandingState::btnNoClick);
	_btnNo->onKeyboardPress((ActionHandler)&ConfirmLandingState::btnNoClick, Options::keyCancel);

	_txtMessage->setColor(Palette::blockOffset(8)+10);
	_txtMessage->setSecondaryColor(Palette::blockOffset(8)+5);
	_txtMessage->setBig();
	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setWordWrap(true);
	_txtMessage->setText(tr("STR_CRAFT_READY_TO_LAND_NEAR_DESTINATION")
						 .arg(_craft->getName(_game->getLanguage()))
						 .arg(_craft->getDestination()->getName(_game->getLanguage())));

	_txtBegin->setColor(Palette::blockOffset(8)+5);
	_txtBegin->setBig();
	_txtBegin->setAlign(ALIGN_CENTER);
	_txtBegin->setText(tr("STR_BEGIN_MISSION"));
}

/**
 *
 */
ConfirmLandingState::~ConfirmLandingState()
{

}

/*
 * Make sure we aren't returning to base.
 */
void ConfirmLandingState::init()
{
	State::init();
	Base* b = dynamic_cast<Base*>(_craft->getDestination());
	if (b == _craft->getBase())
		_game->popState();
}

/**
 * Enters the mission.
 * @param action Pointer to an action.
 */
void ConfirmLandingState::btnYesClick(Action *)
{
	_game->popState();
	Ufo* u = dynamic_cast<Ufo*>(_craft->getDestination());
	TerrorSite* t = dynamic_cast<TerrorSite*>(_craft->getDestination());
	AlienBase* b = dynamic_cast<AlienBase*>(_craft->getDestination());
	ScriptedEventLocation *se = dynamic_cast<ScriptedEventLocation*>(_craft->getDestination());

	SavedBattleGame *bgame = new SavedBattleGame();
	RuleMissionSequence *sequenceRules = 0;
	_game->getSavedGame()->setBattleGame(bgame);
	BattlescapeGenerator bgen(_game);
	bgen.setWorldTexture(_texture);
	bgen.setWorldShade(_shade);
	bgen.setCraft(_craft);

	if (u != 0)
	{
		if(u->getStatus() == Ufo::CRASHED)
			bgame->setMissionType("STR_UFO_CRASH_RECOVERY");
		else
			bgame->setMissionType("STR_UFO_GROUND_ASSAULT");

		if (u->getScriptedEvent())
		{
			// For a scripted event, the briefing screens, alien race and alien deployment rules
			// should come from a missionSequence.
			bgame->setScriptedEventType(u->getScriptedEvent()->getType());
			sequenceRules = _game->getRuleset()->getMissionSequence(u->getScriptedEvent()->getType());
		}
		else
		{
			// For normal UFOs, the UFO has the race and the alien deployment rules use
			// the UFO type.
			bgen.setAlienRace(u->getAlienRace());
		}

		bgen.setUfo(u);
	}
	else if (t != 0)
	{
		bgame->setMissionType("STR_TERROR_MISSION");
		bgen.setTerrorSite(t);
		bgen.setAlienRace(t->getAlienRace());
	}
	else if (b != 0)
	{
		bgame->setMissionType("STR_ALIEN_BASE_ASSAULT");
		bgen.setAlienBase(b);
		bgen.setAlienRace(b->getAlienRace());
	}
	else if (se != 0)
	{
		bgame->setScriptedEventType(se->getScriptedEvent()->getType());
		sequenceRules = _game->getRuleset()->getMissionSequence(se->getScriptedEvent()->getType());
	}
	else
	{
		throw Exception("No mission available!");
	}

	// Look up any multi-mission code for this type of event.
	if (sequenceRules)
	{
		bgame->setMissionSequenceType(sequenceRules->getType());
		bgame->setMissionType(sequenceRules->getMission(0).deployment_id);

		if (sequenceRules->getMission(0).race_id != "") bgen.setAlienRace(sequenceRules->getMission(0).race_id);
		if (sequenceRules->getMission(0).shade > 0) bgen.setWorldShade(sequenceRules->getMission(0).shade);
	}

	bgen.run();
	_game->pushState(new BriefingState(_craft));
}

/**
 * Returns the craft to base and closes the window.
 * @param action Pointer to an action.
 */
void ConfirmLandingState::btnNoClick(Action *)
{
	_craft->returnToBase();
	_game->popState();
}

}
