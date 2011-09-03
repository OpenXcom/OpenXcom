#include "ProductionState.h"
#include "../Interface/Window.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Interface/ArrowButton.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/RuleManufactureInfo.h"
#include "../Savegame/Base.h"
#include "../Savegame/Production.h"
#include "../Engine/Timer.h"
#include "ManufactureState.h"

namespace OpenXcom
{
int getFreeEngineers (Base * base)
{
	int freeEngineers = base->getEngineers();
	const std::vector<Production *> & productions (base->getProductions());
	for (std::vector<Production *>::const_iterator itProduction = productions.begin (); itProduction != productions.end (); ++itProduction)
	{
		freeEngineers -= (*itProduction)->getAssignedEngineers ();
	}
       
	return freeEngineers;
}

ProductionState::ProductionState (Game * game, Base * base, RuleItem * item, ManufactureState * manufactureState) : State (game), _base(base), _item(item), _production(0), _manufactureState(manufactureState)
{
	buildUi();
}

ProductionState::ProductionState (Game * game, Base * base, Production * production, ManufactureState * manufactureState) : State (game), _base(base), _item(0), _production(production), _manufactureState(manufactureState)
{
	buildUi();
}

void ProductionState::buildUi()
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
	_txtTitle = new Text (width - 4 * button_x_border, button_height * 2, start_x + button_x_border, start_y + button_y_border);
	_btnOk = new TextButton (button_width, button_height, width - button_width - button_x_border, start_y + height - button_height - button_y_border);
	_btnStop = new TextButton (button_width, button_height, start_x + button_x_border, start_y + height - button_height - button_y_border);
	_txtAvailableEngineer = new Text(width - 4 * button_x_border, button_height, start_x + button_x_border, start_y + 2 * button_height);
	_txtAvailableSpace = new Text(width - 4 * button_x_border, button_height, start_x + button_x_border, start_y + 2.7f * button_height);
	_txtAllocatedEngineer = new Text(button_width, 2*button_height, start_x + button_x_border, start_y + 3.5f * button_height);
	_txtUnitToProduce = new Text(button_width, 2*button_height, width - button_width - button_x_border, start_y + 3.5f * button_height);
	_txtEngineerUp = new Text(button_width, 2*button_height, start_x + 3*button_x_border, start_y + 6 * button_height);
	_txtEngineerDown = new Text(button_width, 2*button_height, start_x + 3*button_x_border, start_y + 7.5f * button_height);
	_txtUnitUp = new Text(button_width, 2*button_height, width - button_width - button_x_border + 3*button_x_border, start_y + 6 * button_height);
	_txtUnitDown = new Text(button_width, 2*button_height, width - button_width - button_x_border + 3*button_x_border, start_y + 7.5f * button_height);
	_btnEngineerUp = new ArrowButton (ARROW_BIG_UP, 1.5f*button_x_border, button_height, width - button_width - 4*button_x_border, start_y + 6 * button_height);
	_btnEngineerDown = new ArrowButton (ARROW_BIG_DOWN, 1.5f*button_x_border, button_height, width - button_width - 4*button_x_border, start_y + 7.5f * button_height);
	_btnUnitUp = new ArrowButton (ARROW_BIG_UP, 1.5f*button_x_border, button_height, width - 4*button_x_border, start_y + 6 * button_height);
	_btnUnitDown = new ArrowButton (ARROW_BIG_DOWN, 1.5f*button_x_border, button_height, width - 4*button_x_border, start_y + 7.5f * button_height);
	_txtAllocated = new Text(button_width, 2*button_height, width - button_width - 5*button_x_border, start_y + 4 * button_height);
	_txtTodo = new Text(button_width, 2*button_height, width - 5*button_x_border, start_y + 4 * button_height);
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(1)), Palette::backPos, 16);
	
	add(_window);
	add(_txtTitle);
	add(_txtAvailableEngineer);
	add(_txtAvailableSpace);
	add(_txtAllocatedEngineer);
	add(_txtAllocated);
	add(_txtUnitToProduce);
	add(_txtTodo);
	add(_txtEngineerUp);
	add(_txtEngineerDown);
	add(_btnEngineerUp);
	add(_btnEngineerDown);
	add(_txtUnitUp);
	add(_txtUnitDown);
	add(_btnUnitUp);
	add(_btnUnitDown);
	add(_btnOk);
	add(_btnStop);
	_window->setColor(Palette::blockOffset(15)+4);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK17.SCR"));
	_txtTitle->setColor(Palette::blockOffset(15)+1);
	_txtTitle->setText(_game->getLanguage()->getString(_item ? _item->getType() : _production->getRuleItem()->getType()));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_txtAvailableEngineer->setColor(Palette::blockOffset(15)+1);
	_txtAvailableSpace->setColor(Palette::blockOffset(15)+1);
	
	_txtAllocatedEngineer->setColor(Palette::blockOffset(15)+1);
	_txtAllocatedEngineer->setText(_game->getLanguage()->getString("STR_ENGINEERS__ALLOCATED"));
	_txtAllocatedEngineer->setBig();

	_txtAllocated->setColor(Palette::blockOffset(15)+1);
	_txtAllocated->setSecondaryColor(Palette::blockOffset(13));
	_txtAllocated->setBig();
	_txtTodo->setColor(Palette::blockOffset(15)+1);
	_txtTodo->setSecondaryColor(Palette::blockOffset(13));
	_txtTodo->setBig();

	_txtUnitToProduce->setColor(Palette::blockOffset(15)+1);
	_txtUnitToProduce->setText(_game->getLanguage()->getString("STR_TOTAL_TO_PRODUCE"));
	_txtUnitToProduce->setBig();

	_txtEngineerUp->setColor(Palette::blockOffset(15)+1);
	_txtEngineerUp->setText(_game->getLanguage()->getString("STR_INCREASE_UC"));
	_txtEngineerDown->setColor(Palette::blockOffset(15)+1);
	_txtEngineerDown->setText(_game->getLanguage()->getString("STR_DECREASE_UC"));
	_btnEngineerUp->setColor(Palette::blockOffset(15)+4);
	_btnEngineerUp->onMousePress((ActionHandler)&ProductionState::moreEngineerPress);
	_btnEngineerUp->onMouseRelease((ActionHandler)&ProductionState::moreEngineerRelease);

	_btnEngineerDown->setColor(Palette::blockOffset(15)+4);
	_btnEngineerDown->onMousePress((ActionHandler)&ProductionState::lessEngineerPress);
	_btnEngineerDown->onMouseRelease((ActionHandler)&ProductionState::lessEngineerRelease);

	_btnUnitUp->setColor(Palette::blockOffset(15)+4);
	_btnUnitUp->onMousePress((ActionHandler)&ProductionState::moreUnitPress);
	_btnUnitUp->onMouseRelease((ActionHandler)&ProductionState::moreUnitRelease);

	_btnUnitDown->setColor(Palette::blockOffset(15)+4);
	_btnUnitDown->onMousePress((ActionHandler)&ProductionState::lessUnitPress);
	_btnUnitDown->onMouseRelease((ActionHandler)&ProductionState::lessUnitRelease);

	_txtUnitUp->setColor(Palette::blockOffset(15)+1);
	_txtUnitUp->setText(_game->getLanguage()->getString("STR_INCREASE_UC"));
	_txtUnitDown->setColor(Palette::blockOffset(15)+1);
	_txtUnitDown->setText(_game->getLanguage()->getString("STR_DECREASE_UC"));

	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ProductionState::btnOkClick);

	_btnStop->setColor(Palette::blockOffset(13)+13);
	_btnStop->setText(_game->getLanguage()->getString("STR_STOP_PRODUCTION"));
	_btnStop->onMouseClick((ActionHandler)&ProductionState::btnStopClick);
	if(!_production)
	{
		_production = new Production (_item, 0);
		_base->addProduction(_production);
	}
	setAssignedEngineer();
	
	_timerMoreEngineer = new Timer(30);
	_timerLessEngineer = new Timer(30);
	_timerMoreUnit = new Timer(30);
	_timerLessUnit = new Timer(30);
	_timerMoreEngineer->onTimer((StateHandler)&ProductionState::onMoreEngineer);
	_timerLessEngineer->onTimer((StateHandler)&ProductionState::onLessEngineer);
	_timerMoreUnit->onTimer((StateHandler)&ProductionState::onMoreUnit);
	_timerLessUnit->onTimer((StateHandler)&ProductionState::onLessUnit);
}

void ProductionState::btnStopClick (Action * action)
{
	exitState();
}

void ProductionState::btnOkClick (Action * action)
{
	exitState();
}

void ProductionState::exitState()
{
	_manufactureState->fillProductionList();
	_game->popState();
	if(_item)
	{
		_game->popState();
	}
}

void ProductionState::setAssignedEngineer()
{
	int availableEngineer = getFreeEngineers(_base);
	int availableWorkSpace = _base->getAvailableWorkshops() - _base->getUsedWorkshops();
	std::wstringstream s1;
	s1 << _game->getLanguage()->getString("STR_ENGINEERS_AVAILABLE_UC") << availableEngineer;
	_txtAvailableEngineer->setText(s1.str());
	std::wstringstream s2;
	s2 << _game->getLanguage()->getString("STR_SPACE_AVAILABLE") << availableWorkSpace;
	_txtAvailableSpace->setText(s2.str());
	std::wstringstream s3;
	s3 << ">" << _production->getAssignedEngineers();
	_txtAllocated->setText(s3.str());
	std::wstringstream s4;
	s4 << ">" << _production->getNumberOfItemTodo ();
	_txtTodo->setText(s4.str());
}

void ProductionState::moreEngineerPress(Action * action)
{
	_timerMoreEngineer->start();
}

void ProductionState::moreEngineerRelease(Action * action)
{
	_timerMoreEngineer->stop();
}

void ProductionState::lessEngineerPress(Action * action)
{
	_timerLessEngineer->start();
}

void ProductionState::lessEngineerRelease(Action * action)
{
	_timerLessEngineer->stop();
}

void ProductionState::moreUnitPress(Action * action)
{
	_timerMoreUnit->start();
}

void ProductionState::moreUnitRelease(Action * action)
{
	_timerMoreUnit->stop();
}

void ProductionState::lessUnitPress(Action * action)
{
	_timerLessUnit->start();
}

void ProductionState::lessUnitRelease(Action * action)
{
	_timerMoreUnit->stop();
}

void ProductionState::onMoreEngineer(Action * action)
{
	int assigned = _production->getAssignedEngineers();
	int availableEngineer = getFreeEngineers(_base);
	int availableWorkSpace = _base->getAvailableWorkshops() - _base->getUsedWorkshops();
	if (availableEngineer > 0 && availableWorkSpace > 0)
	{
		_production->setAssignedEngineers(++assigned);
		setAssignedEngineer();
	}
}

void ProductionState::onLessEngineer(Action * action)
{
	int assigned = _production->getAssignedEngineers();
	if(assigned > 0)
	{
		_production->setAssignedEngineers(--assigned);
		setAssignedEngineer();
	}
}

void ProductionState::onMoreUnit(Action * action)
{
	int more = _production->getNumberOfItemTodo ();
	_production->setNumberOfItemTodo (++more);
	setAssignedEngineer();	
}

void ProductionState::onLessUnit(Action * action)
{
	int less = _production->getNumberOfItemTodo ();
	if(less > (_production->getNumberOfItemDone () + 1))
	{
		_production->setNumberOfItemTodo (--less);
		setAssignedEngineer();	
	}
}

void ProductionState::think()
{
	State::think();
	_timerMoreEngineer->think(this, 0);
	_timerLessEngineer->think(this, 0);
	_timerMoreUnit->think(this, 0);
	_timerLessUnit->think(this, 0);
}
}
