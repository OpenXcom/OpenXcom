/*
 * Copyright 2010-2012 OpenXcom Developers.
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

#ifndef OPENXCOM_UFOPAEDIASTARTSTATE_H
#define OPENXCOM_UFOPAEDIASTARTSTATE_H

#include "../Engine/State.h"
#include "Ufopaedia.h"
#include <string>

namespace OpenXcom
{
	class Game;
	class Action;
	class Window;
	class Text;
	class TextButton;

	/**
	 * UfopaediaStartState is the screen that opens when clicking Ufopaedia button in Geoscape.
	 * Presents buttons to all sections of Ufopaedia, opening a UfopaediaSelectState on click.
	 */

	class UfopaediaStartState : public State
	{
	public:
		UfopaediaStartState(Game *game);
		virtual ~UfopaediaStartState();

	protected:
		Window *_window;
		Text *_txtTitle;
		TextButton *_btnOk;
		TextButton *_btnCraftArmament;
		TextButton *_btnHWP;
		TextButton *_btnWeaponsEquipment;
		TextButton *_btnAlienArtefacts;
		TextButton *_btnBaseFacilities;
		TextButton *_btnAlienLifeforms;
		TextButton *_btnAlienResearch;
		TextButton *_btnUfoComponents;
		TextButton *_btnUfos;

		// navigation callbacks
		void btnOkClick(Action *action);
		void btnCraftArmamentClick(Action *action);
		void btnWeaponsEquipmentClick(Action *action);
		void btnBaseFacilitiesClick(Action *action);
		void btnAlienLifeformsClick(Action *action);
		void btnAlienResearchClick(Action *action);
		void btnUfosClick(Action *action);
//		void btnClick(Action *action);
	};
}

#endif
