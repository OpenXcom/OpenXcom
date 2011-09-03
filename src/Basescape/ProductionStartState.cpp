#include "ProductionStartState.h"
#include "../Interface/Window.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/RuleManufactureInfo.h"
#include "../Savegame/Base.h"
#include "../Savegame/ItemContainer.h"
#include <sstream>

namespace OpenXcom
{
ProductionStartState::ProductionStartState(Game * game, Base * base, RuleItem * item) : State(game), _base(base), _item(item)
{
	_screen = false;
	int width = 320;
	int height = 170;
	int max_width = 320;
	int max_height = 200;
	int start_x = (max_width - width) / 2;
	int start_y = (max_height - height) / 2;
	int button_x_border = 10;
	int button_y_border = 10;
	int button_height = 16;

	int button_width = (width - 5 * button_x_border) / 2;
	_window = new Window(this, width, height, start_x, start_y, POPUP_BOTH);
	_btnCancel = new TextButton (button_width, button_height, start_x + button_x_border, start_y + height - button_height - button_y_border);
	_txtTitle = new Text (width - 4 * button_x_border, button_height * 2, start_x + button_x_border, start_y + button_y_border);
	_txtManHour = new Text (width - 4 * button_x_border, button_height, start_x + button_x_border * 2, start_y + button_y_border * 3);
	_txtCost = new Text (width - 4 * button_x_border, button_height, start_x + button_x_border * 2, start_y + button_y_border * 4);
	_txtWorkSpace = new Text (width - 4 * button_x_border, button_height, start_x + button_x_border * 2, start_y + button_y_border * 5);
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(1)), Palette::backPos, 16);
	
	add(_window);
	add(_txtTitle);
	add(_txtManHour);
	add(_txtCost);
	add(_txtWorkSpace);
	add(_btnCancel);

	_window->setColor(Palette::blockOffset(13)+13);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK17.SCR"));

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setText(_game->getLanguage()->getString(_item->getType()));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_txtManHour->setColor(Palette::blockOffset(13)+10);
	std::wstringstream sstr;
	sstr << _item->getManufactureInfo()->getManufactureTime () << L" " << _game->getLanguage()->getString("STR_ENGINEER_HOURS_TO_PRODUCE_ONE_UNIT");
	_txtManHour->setText(sstr.str());

	std::wstringstream sstr1;
	sstr1 << _game->getLanguage()->getString("STR_COST_PER_UNIT_") << _item->getManufactureInfo()->getManufactureCost ();
	_txtCost->setColor(Palette::blockOffset(13)+10);
	_txtCost->setSecondaryColor(Palette::blockOffset(13));
	_txtCost->setText(sstr1.str());

	std::wstringstream sstr2;
	sstr2 << _game->getLanguage()->getString("STR_WORK_SPACE_REQUIRED") << _item->getManufactureInfo()->getRequiredSpace ();
	_txtWorkSpace->setColor(Palette::blockOffset(13)+10);
	_txtWorkSpace->setSecondaryColor(Palette::blockOffset(13));
	_txtWorkSpace->setText(sstr2.str());

	_btnCancel->setColor(Palette::blockOffset(13)+13);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&ProductionStartState::btnCancelClick);
	
	const std::map<std::string, int> & neededItems (_item->getManufactureInfo()->getNeededItems());
	bool productionPossible (true);
	if (!neededItems.empty())
	{
		_txtNeedeItemsTitle = new Text (width - 4 * button_x_border, button_height, start_x + button_x_border * 2, start_y + button_y_border * 6);
		_txtItemNameColumn = new Text (6 * button_x_border, button_height, start_x + button_x_border * 3, start_y + button_y_border * 7);
		_txtUnitRequiredColumn = new Text (6 * button_x_border, button_height, start_x + button_x_border * 14, start_y + button_y_border * 7);
		_txtUnitAvailableColumn = new Text (6 * button_x_border, button_height, start_x + button_x_border * 22, start_y + button_y_border * 7);
		_lstNeededItems = new TextList(width - 8 * button_x_border, height - (start_y + button_y_border * 11), start_x + button_x_border * 3, start_y + button_y_border * 9);
		_txtNeedeItemsTitle->setColor(Palette::blockOffset(13)+10);
		_txtNeedeItemsTitle->setText(_game->getLanguage()->getString("STR_SPECIAL_MATERIALS_REQUIRED"));
		_txtNeedeItemsTitle->setAlign(ALIGN_CENTER);

		_txtItemNameColumn->setColor(Palette::blockOffset(13)+10);
		_txtItemNameColumn->setText(_game->getLanguage()->getString("STR_ITEM_REQUIRED"));
		_txtUnitRequiredColumn->setColor(Palette::blockOffset(13)+10);
		_txtUnitRequiredColumn->setText(_game->getLanguage()->getString("STR_UNITS_REQUIRED"));
		_txtUnitAvailableColumn->setColor(Palette::blockOffset(13)+10);
		_txtUnitAvailableColumn->setText(_game->getLanguage()->getString("STR_UNITS_AVAILABLE"));
	
		_lstNeededItems->setColumns(3, 12 * button_x_border, 8 * button_x_border, 8 * button_x_border);
		_lstNeededItems->setBackground(_window);
		_lstNeededItems->setMargin(2);
		_lstNeededItems->setColor(Palette::blockOffset(13));
		_lstNeededItems->setArrowColor(Palette::blockOffset(13));

		add(_txtNeedeItemsTitle);
		add(_txtItemNameColumn);
		add(_txtUnitRequiredColumn);
		add(_txtUnitAvailableColumn);
		add(_lstNeededItems);
		ItemContainer * itemContainer (base->getItems());
		for(std::map<std::string, int>::const_iterator iter = neededItems.begin ();
		    iter != neededItems.end ();
		    ++iter)
		{
			std::wstringstream s1, s2;
			s1 << iter->second;
			s2 << itemContainer->getItem(iter->first);
			productionPossible &= (itemContainer->getItem(iter->first) >= iter->second);
			_lstNeededItems->addRow(3, _game->getLanguage()->getString(iter->first).c_str(), s1.str().c_str(), s2.str().c_str());
		}
	}
	if(productionPossible)
	{
		_btnStart = new TextButton (button_width, button_height, width - button_width - button_x_border, start_y + height - button_height - button_y_border);
		add(_btnStart);
		_btnStart->setColor(Palette::blockOffset(13)+13);
		_btnStart->setText(_game->getLanguage()->getString("STR_START_PRODUCTION"));
		_btnStart->onMouseClick((ActionHandler)&ProductionStartState::btnStartClick);
	}
}

void ProductionStartState::btnCancelClick(Action * action)
{
	_game->popState();
}

void ProductionStartState::btnStartClick(Action * action)
{
}
}
