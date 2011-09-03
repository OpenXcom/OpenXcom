#include "ListPossibleProductionState.h"
#include "../Interface/Window.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/Ruleset.h"
#include "ProductionStartState.h"
#include "../Savegame/Production.h"
#include "../Savegame/Base.h"
#include <algorithm>

namespace OpenXcom
{
struct equalProduction : public std::unary_function<Production *,
						    bool>
{
	RuleItem * _item;
	equalProduction(RuleItem * item);
	bool operator()(const Production * p) const;
};

equalProduction::equalProduction(RuleItem * item) : _item(item)
{
}

bool equalProduction::operator()(const Production * p) const
{
	return p->getRuleItem() == _item;
}

ListPossibleProductionState::ListPossibleProductionState(Game *game, Base *base, ManufactureState * manufactureState) : State(game), _base(base), _manufactureState(manufactureState)
{
	int width = 320;
	int height = 140;
	int max_width = 320;
	int max_height = 200;
	int start_x = (max_width - width) / 2;
	int start_y = (max_height - height) / 2;
	int button_x_border = 10;
	int button_y_border = 10;
	int button_height = 16;
	_screen = false;
	_window = new Window(this, width, height, start_x, start_y, POPUP_BOTH);
	_btnOk = new TextButton (width - 2 * button_x_border , button_height, start_x + button_x_border, start_y + height - button_height - button_y_border);
	_txtTitle = new Text (width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + button_y_border);
	_txtItem = new Text (5 * button_x_border, button_height, start_x + button_x_border, start_y + 3 * button_y_border);
	_txtCategory = new Text (5 * button_x_border, button_height, start_x + 17 * button_x_border, start_y + 3 * button_y_border);
	_lstManufacture = new TextList(width - 4 * button_x_border, height - 3.1f * button_height - 2 * button_y_border, start_x + button_x_border, start_y + 2.5f * button_height);

	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(1)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtItem);
	add(_txtCategory);
	add(_lstManufacture);

	_window->setColor(Palette::blockOffset(15)+4);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK17.SCR"));
	_txtTitle->setColor(Palette::blockOffset(15)+1);
	_txtTitle->setText(_game->getLanguage()->getString("STR_PRODUCTION_ITEMS"));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_txtItem->setColor(Palette::blockOffset(15)+1);
	_txtItem->setText(_game->getLanguage()->getString("STR_ITEM"));

	_txtCategory->setColor(Palette::blockOffset(15)+1);
	_txtCategory->setText(_game->getLanguage()->getString("STR_CATEGORY"));

	_lstManufacture->setColumns(2, 16 * button_x_border, 5 * button_x_border);
	_lstManufacture->setSelectable(true);
	_lstManufacture->setBackground(_window);
	_lstManufacture->setMargin(2);
	_lstManufacture->setArrowColor(Palette::blockOffset(15));
	_lstManufacture->setColor(Palette::blockOffset(13));
	_lstManufacture->onMouseClick((ActionHandler)&ListPossibleProductionState::lstProdClick);

	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ListPossibleProductionState::btnOkClick);
	fillProductionList();
}

void ListPossibleProductionState::btnOkClick(Action * action)
{
	_game->popState();
}

void ListPossibleProductionState::lstProdClick (Action * action)
{
	_game->pushState(new ProductionStartState(_game, _base, _possibleProductions[_lstManufacture->getSelectedRow()], _manufactureState));
}

void ListPossibleProductionState::fillProductionList()
{
	_lstManufacture->clearList();
	const std::map<std::string, RuleItem *> & items (_game->getRuleset()->getItems ());
	const std::vector<Production *> productions (_base->getProductions ());

	for(std::map<std::string, RuleItem *>::const_iterator iter = items.begin ();
	    iter != items.end ();
	    ++iter)
	{
		if(!iter->second->getManufactureInfo())
		{
			continue;
		}
		 if(std::find_if(productions.begin (), productions.end (), equalProduction(iter->second)) != productions.end ())
		{
			continue;
		}

		_lstManufacture->addRow(2, _game->getLanguage()->getString(iter->first).c_str(), L"Unknown");
		_possibleProductions.push_back(iter->second);
	}
}
}
