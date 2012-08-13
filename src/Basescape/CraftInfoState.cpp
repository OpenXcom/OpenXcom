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
#include "CraftInfoState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/SurfaceSet.h"
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
	_btnW1 = new TextButton(24, 32, 24, 48);
	_btnW2 = new TextButton(24, 32, 272, 48);
	_btnCrew = new TextButton(64, 16, 24, 96);
	_btnEquip = new TextButton(64, 16, 24, 120);
	_btnArmor = new TextButton(64, 16, 24, 144);
	_txtCraft = new Text(310, 16, 5, 8);
	_txtDamage = new Text(82, 9, 24, 24);
	_txtFuel = new Text(82, 9, 232, 24);
	_txtW1Name = new Text(90, 9, 56, 48);
	_txtW1Ammo = new Text(60, 9, 56, 64);
	_txtW1Max = new Text(60, 9, 56, 72);
	_txtW2Name = new Text(90, 9, 204, 48);
	_txtW2Ammo = new Text(60, 9, 204, 64);
	_txtW2Max = new Text(60, 9, 204, 72);
	_sprite = new Surface(32, 32, 144, 52);
	_weapon1 = new Surface(15, 17, 121, 63);
	_weapon2 = new Surface(15, 17, 184, 63);
	_crew = new Surface(210, 18, 95, 96);
	_equip = new Surface(210, 18, 95, 121);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(3)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnW1);
	add(_btnW2);
	add(_btnCrew);
	add(_btnEquip);
	add(_btnArmor);
	add(_txtCraft);
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

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+10);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK14.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CraftInfoState::btnOkClick);

	_btnW1->setColor(Palette::blockOffset(13)+10);
	_btnW1->setText(L"1");
	_btnW1->onMouseClick((ActionHandler)&CraftInfoState::btnW1Click);

	_btnW2->setColor(Palette::blockOffset(13)+10);
	_btnW2->setText(L"2");
	_btnW2->onMouseClick((ActionHandler)&CraftInfoState::btnW2Click);

	_btnCrew->setColor(Palette::blockOffset(13)+10);
	_btnCrew->setText(_game->getLanguage()->getString("STR_CREW"));
	_btnCrew->onMouseClick((ActionHandler)&CraftInfoState::btnCrewClick);

	_btnEquip->setColor(Palette::blockOffset(13)+10);
	_btnEquip->setText(_game->getLanguage()->getString("STR_EQUIPMENT_UC"));
	_btnEquip->onMouseClick((ActionHandler)&CraftInfoState::btnEquipClick);

	_btnArmor->setColor(Palette::blockOffset(13)+10);
	_btnArmor->setText(_game->getLanguage()->getString("STR_ARMOR"));
	_btnArmor->onMouseClick((ActionHandler)&CraftInfoState::btnArmorClick);

	_txtCraft->setColor(Palette::blockOffset(13)+10);
	_txtCraft->setBig();
	_txtCraft->setAlign(ALIGN_CENTER);

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

	_txtCraft->setText(c->getName(_game->getLanguage()));

	SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
	texture->getFrame(c->getRules()->getSprite() + 33)->setX(0);
	texture->getFrame(c->getRules()->getSprite() + 33)->setY(0);
	texture->getFrame(c->getRules()->getSprite() + 33)->blit(_sprite);

	std::wstringstream ss;
	ss << _game->getLanguage()->getString("STR_DAMAGE_UC_") << L'\x01' << c->getDamagePercentage() << "%";
	_txtDamage->setText(ss.str());

	std::wstringstream ss2;
	ss2 << _game->getLanguage()->getString("STR_FUEL") << L'\x01' << c->getFuelPercentage() << "%";
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

		Surface *frame2 = texture->getFrame(39);
		frame2->setY(0);
		int x = 0;
		for (int i = 0; i < c->getNumEquipment(); i += 4, x += 10)
		{
			frame2->setX(x);
			frame2->blit(_equip);
		}
		Surface *frame3 = texture->getFrame(40);
		for (int i = 0; i < c->getNumVehicles(); ++i, x += 10)
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

			_txtW1Name->setText(_game->getLanguage()->getString(w1->getRules()->getType()));

			std::wstringstream ss3;
			ss3 << _game->getLanguage()->getString("STR_AMMO_") << L'\x01' << w1->getAmmo();
			_txtW1Ammo->setText(ss3.str());

			std::wstringstream ss4;
			ss4 << _game->getLanguage()->getString("STR_MAX") << L'\x01' << w1->getRules()->getAmmoMax();
			_txtW1Max->setText(ss4.str());
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

			_txtW2Name->setText(_game->getLanguage()->getString(w2->getRules()->getType()));

			std::wstringstream ss5;
			ss5 << _game->getLanguage()->getString("STR_AMMO_") << L'\x01' << w2->getAmmo();
			_txtW2Ammo->setText(ss5.str());

			std::wstringstream ss6;
			ss6 << _game->getLanguage()->getString("STR_MAX") << L'\x01' << w2->getRules()->getAmmoMax();
			_txtW2Max->setText(ss6.str());
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
void CraftInfoState::btnOkClick(Action *action)
{
	_game->popState();
}

/**
 * Goes to the Select Armament window for
 * the first weapon.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnW1Click(Action *action)
{
	_game->pushState(new CraftWeaponsState(_game, _base, _craft, 0));
}

/**
 * Goes to the Select Armament window for
 * the second weapon.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnW2Click(Action *action)
{
	_game->pushState(new CraftWeaponsState(_game, _base, _craft, 1));
}

/**
 * Goes to the Select Squad screen.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnCrewClick(Action *action)
{
	_game->pushState(new CraftSoldiersState(_game, _base, _craft));
}

/**
 * Goes to the Select Equipment screen.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnEquipClick(Action *action)
{
	_game->pushState(new CraftEquipmentState(_game, _base, _craft));
}

/**
 * Goes to the Select Armor screen.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnArmorClick(Action *action)
{
	_game->pushState(new CraftArmorState(_game, _base, _craft));
}

}
