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
#include "CraftInfoState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Action.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "../Savegame/CraftWeapon.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Savegame/Base.h"
#include "CraftSoldiersState.h"
#include "CraftWeaponsState.h"
#include "CraftEquipmentState.h"
#include "CraftArmorState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Craft Info screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param craft ID of the selected craft.
 */
CraftInfoState::CraftInfoState(Game *game, Base *base, size_t craft) : State(game), _base(base), _craft(craft)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_btnOk = new TextButton(64, 24, 128, 168);
	_btnW1 = new TextButton(24, 32, 14, 48);
	_btnW2 = new TextButton(24, 32, 282, 48);
	_btnCrew = new TextButton(64, 16, 14, 96);
	_btnEquip = new TextButton(64, 16, 14, 120);
	_btnArmor = new TextButton(64, 16, 14, 144);
	_edtCraft = new TextEdit(160, 16, 80, 8);
	_txtDamage = new Text(82, 17, 14, 24);
	_txtFuel = new Text(82, 17, 228, 24);
	_txtW1Name = new Text(90, 9, 46, 48);
	_txtW1Ammo = new Text(60, 9, 46, 64);
	_txtW1Max = new Text(60, 9, 46, 72);
	_txtW2Name = new Text(90, 9, 204, 48);
	_txtW2Ammo = new Text(60, 9, 204, 64);
	_txtW2Max = new Text(60, 9, 204, 72);
	_sprite = new Surface(32, 40, 144, 52);
	_weapon1 = new Surface(15, 17, 121, 63);
	_weapon2 = new Surface(15, 17, 184, 63);
	_crew = new Surface(220, 18, 85, 96);
	_equip = new Surface(220, 18, 85, 121);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_1")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(3)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnW1);
	add(_btnW2);
	add(_btnCrew);
	add(_btnEquip);
	add(_btnArmor);
	add(_edtCraft);
	add(_txtDamage);
	add(_txtFuel);
	add(_txtW1Name);
	add(_txtW1Ammo);
	add(_txtW1Max);
	add(_txtW2Name);
	add(_txtW2Ammo);
	add(_txtW2Max);
	add(_sprite);
	add(_weapon1);
	add(_weapon2);
	add(_crew);
	add(_equip);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+10);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK14.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CraftInfoState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&CraftInfoState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));

	_btnW1->setColor(Palette::blockOffset(13)+10);
	_btnW1->setText(L"1");
	_btnW1->onMouseClick((ActionHandler)&CraftInfoState::btnW1Click);

	_btnW2->setColor(Palette::blockOffset(13)+10);
	_btnW2->setText(L"2");
	_btnW2->onMouseClick((ActionHandler)&CraftInfoState::btnW2Click);

	_btnCrew->setColor(Palette::blockOffset(13)+10);
	_btnCrew->setText(tr("STR_CREW"));
	_btnCrew->onMouseClick((ActionHandler)&CraftInfoState::btnCrewClick);

	_btnEquip->setColor(Palette::blockOffset(13)+10);
	_btnEquip->setText(tr("STR_EQUIPMENT_UC"));
	_btnEquip->onMouseClick((ActionHandler)&CraftInfoState::btnEquipClick);

	_btnArmor->setColor(Palette::blockOffset(13)+10);
	_btnArmor->setText(tr("STR_ARMOR"));
	_btnArmor->onMouseClick((ActionHandler)&CraftInfoState::btnArmorClick);

	_edtCraft->setColor(Palette::blockOffset(13)+10);
	_edtCraft->setBig();
	_edtCraft->setAlign(ALIGN_CENTER);
	_edtCraft->onKeyboardPress((ActionHandler)&CraftInfoState::edtCraftKeyPress);

	_txtDamage->setColor(Palette::blockOffset(13)+10);
	_txtDamage->setSecondaryColor(Palette::blockOffset(13));

	_txtFuel->setColor(Palette::blockOffset(13)+10);
	_txtFuel->setSecondaryColor(Palette::blockOffset(13));

	_txtW1Name->setColor(Palette::blockOffset(13)+5);

	_txtW1Ammo->setColor(Palette::blockOffset(13)+10);
	_txtW1Ammo->setSecondaryColor(Palette::blockOffset(13)+5);

	_txtW1Max->setColor(Palette::blockOffset(13)+10);
	_txtW1Max->setSecondaryColor(Palette::blockOffset(13)+5);

	_txtW2Name->setColor(Palette::blockOffset(13)+5);

	_txtW2Ammo->setColor(Palette::blockOffset(13)+10);
	_txtW2Ammo->setSecondaryColor(Palette::blockOffset(13)+5);

	_txtW2Max->setColor(Palette::blockOffset(13)+10);
	_txtW2Max->setSecondaryColor(Palette::blockOffset(13)+5);
}

/**
 *
 */
CraftInfoState::~CraftInfoState()
{

}

/**
 * The craft info can change
 * after going into other screens.
 */
void CraftInfoState::init()
{
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(3)), Palette::backPos, 16);

	Craft *c = _base->getCrafts()->at(_craft);

	_edtCraft->setText(c->getName(_game->getLanguage()));

	SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
	texture->getFrame(c->getRules()->getSprite() + 33)->setX(0);
	texture->getFrame(c->getRules()->getSprite() + 33)->setY(0);
	texture->getFrame(c->getRules()->getSprite() + 33)->blit(_sprite);

	std::wstringstream ss;
	ss << tr("STR_DAMAGE_UC_").arg(Text::formatPercentage(c->getDamagePercentage()));
	if (c->getStatus() == "STR_REPAIRS")
	{
		int damageDays = (int)ceil((float)c->getDamage() / c->getRules()->getRepairRate() / 24.0f);
		ss << L"\n(" << tr("STR_DAY", damageDays) << ")";
	}
	_txtDamage->setText(ss.str());

	std::wstringstream ss2;
	ss2 << tr("STR_FUEL").arg(Text::formatPercentage(c->getFuelPercentage()));
	if (c->getStatus() == "STR_REFUELLING")
	{
		int fuelDays = (int) ceil((float)(c->getRules()->getMaxFuel() - c->getFuel()) / c->getRules()->getRefuelRate() / 48.0f);
		ss2 << L"\n(" << tr("STR_DAY", fuelDays) << ")";
	}
	_txtFuel->setText(ss2.str());

	if (c->getRules()->getSoldiers() > 0)
	{
		_crew->clear();
		_equip->clear();

		Surface *frame1 = texture->getFrame(38);
		frame1->setY(0);
		for (int i = 0, x = 0; i < c->getNumSoldiers(); ++i, x += 10)
		{
			frame1->setX(x);
			frame1->blit(_crew);
		}

		Surface *frame2 = texture->getFrame(40);
		frame2->setY(0);
		int x = 0;
		for (int i = 0; i < c->getNumVehicles(); ++i, x += 10)
		{
			frame2->setX(x);
			frame2->blit(_equip);
		}
		Surface *frame3 = texture->getFrame(39);
		for (int i = 0; i < c->getNumEquipment(); i += 4, x += 10)
		{
			frame3->setX(x);
			frame3->blit(_equip);
		}
	}
	else
	{
		_crew->setVisible(false);
		_equip->setVisible(false);
		_btnCrew->setVisible(false);
		_btnEquip->setVisible(false);
		_btnArmor->setVisible(false);
	}

	if (c->getRules()->getWeapons() > 0)
	{
		CraftWeapon *w1 = c->getWeapons()->at(0);

		if (w1 != 0)
		{
			Surface *frame = texture->getFrame(w1->getRules()->getSprite() + 48);
			frame->setX(0);
			frame->setY(0);
			frame->blit(_weapon1);

			_txtW1Name->setText(tr(w1->getRules()->getType()));
			_txtW1Ammo->setText(tr("STR_AMMO_").arg(w1->getAmmo()));
			_txtW1Max->setText(tr("STR_MAX").arg(w1->getRules()->getAmmoMax()));
		}
		else
		{
			_weapon1->clear();
			_txtW1Name->setText(L"");
			_txtW1Ammo->setText(L"");
			_txtW1Max->setText(L"");
		}
	}
	else
	{
		_weapon1->setVisible(false);
		_btnW1->setVisible(false);
		_txtW1Name->setVisible(false);
		_txtW1Ammo->setVisible(false);
		_txtW1Max->setVisible(false);
	}

	if (c->getRules()->getWeapons() > 1)
	{
		CraftWeapon *w2 = c->getWeapons()->at(1);

		if (w2 != 0)
		{
			Surface *frame = texture->getFrame(w2->getRules()->getSprite() + 48);
			frame->setX(0);
			frame->setY(0);
			frame->blit(_weapon2);

			_txtW2Name->setText(tr(w2->getRules()->getType()));
			_txtW2Ammo->setText(tr("STR_AMMO_").arg(w2->getAmmo()));
			_txtW2Max->setText(tr("STR_MAX").arg(w2->getRules()->getAmmoMax()));
		}
		else
		{
			_weapon2->clear();
			_txtW2Name->setText(L"");
			_txtW2Ammo->setText(L"");
			_txtW2Max->setText(L"");
		}
	}
	else
	{
		_weapon2->setVisible(false);
		_btnW2->setVisible(false);
		_txtW2Name->setVisible(false);
		_txtW2Ammo->setVisible(false);
		_txtW2Max->setVisible(false);
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnOkClick(Action *)
{
	Craft *c = _base->getCrafts()->at(_craft);
	if (c->getName(_game->getLanguage()) != _edtCraft->getText())
	{
		c->setName(_edtCraft->getText());
	}
	_game->popState();
}

/**
 * Goes to the Select Armament window for
 * the first weapon.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnW1Click(Action *)
{
	_game->pushState(new CraftWeaponsState(_game, _base, _craft, 0));
}

/**
 * Goes to the Select Armament window for
 * the second weapon.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnW2Click(Action *)
{
	_game->pushState(new CraftWeaponsState(_game, _base, _craft, 1));
}

/**
 * Goes to the Select Squad screen.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnCrewClick(Action *)
{
	_game->pushState(new CraftSoldiersState(_game, _base, _craft));
}

/**
 * Goes to the Select Equipment screen.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnEquipClick(Action *)
{
	_game->pushState(new CraftEquipmentState(_game, _base, _craft));
}

/**
 * Goes to the Select Armor screen.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnArmorClick(Action *)
{
	_game->pushState(new CraftArmorState(_game, _base, _craft));
}

/**
 * Changes the Craft name.
 * @param action Pointer to an action.
 */
void CraftInfoState::edtCraftKeyPress(Action *action)
{
	if (action->getDetails()->key.keysym.sym == SDLK_RETURN ||
		action->getDetails()->key.keysym.sym == SDLK_KP_ENTER)
	{
		Craft *c = _base->getCrafts()->at(_craft);
		if (c->getName(_game->getLanguage()) != _edtCraft->getText())
		{
			c->setName(_edtCraft->getText());
			_edtCraft->setText(c->getName(_game->getLanguage()));
		}
	}
}
}
