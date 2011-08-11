#include "EndResearchState.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"

namespace OpenXcom
{
  EndResearchState::EndResearchState(Game * game) : State (game)
  {
	_screen = false;

	// Create objects
	_window = new Window(this, 210, 140, 70, 30, POPUP_BOTH);
	_btnOk = new TextButton(80, 16, 80, 145);
	_btnOk5Secs = new TextButton(80, 16, 190, 145);
	_txtTitle = new Text(200, 16, 80, 90);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnOk5Secs);
	add(_txtTitle);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	//_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+8);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	// _btnOk->onMouseClick((ActionHandler)&ItemsArrivingState::btnOkClick);

	_btnOk5Secs->setColor(Palette::blockOffset(8)+8);
	//_btnOk5Secs->setText(_game->getLanguage()->getString("STR_OK_5_SECS"));
	_btnOk5Secs->setText(_game->getLanguage()->getString("STR_VIEW_REPORTS"));
	// _btnOk5Secs->onMouseClick((ActionHandler)&ItemsArrivingState::btnOk5SecsClick);

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	//_txtTitle->setText(_game->getLanguage()->getString("STR_ITEMS_ARRIVING"));
	_txtTitle->setText(_game->getLanguage()->getString("STR_RESEARCH_COMPLETED"));
	// _txtEndResearch->setText (_game->getLanguage()->getString("STR_RESEARCH_COMPLETED"));

	// int width = 220;
	// int height = 140;
	// int max_width = 320;
	// int max_height = 200;
	// int start_x = (max_width - width) / 2;
	// int start_y = (max_height - height) / 2;
	
	// int button_x_border = 10;
	// int button_y_border = 10;
	// int button_height = 16;
	// int button_width = 80;
	// _screen = false;
	// _window = new Window(this, width, height, start_x, start_y/*, POPUP_BOTH*/);
	// _btnOk = new TextButton (button_width , button_height, start_x + button_x_border, start_y + height - button_height - button_y_border);
	// _btnReport = new TextButton (button_width, button_height, start_x + width - button_x_border - button_width, start_y + height - button_height - button_y_border);
	// _txtEndResearch = new Text (width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + height/2);
	// //_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(1)), Palette::backPos, 16);
	// _game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	// //_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	// add(_btnOk);
	// add(_btnReport);
	// add(_txtEndResearch);

	// _window->setColor(Palette::blockOffset(15)-1);
	// //_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));
	// _window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));

	// _btnOk->setColor(Palette::blockOffset(15)-1);
	// _btnOk->setText(_game->getLanguage()->getString("STR_OK"));

	// _btnReport->setColor(Palette::blockOffset(15)-1);
	// _btnReport->setText(_game->getLanguage()->getString("STR_VIEW_REPORTS"));

	// _txtEndResearch->setColor(Palette::blockOffset(15)-1);
	// _txtEndResearch->setAlign(ALIGN_CENTER);
	// _txtEndResearch->setText (_game->getLanguage()->getString("STR_RESEARCH_COMPLETED"));
  }

	void EndResearchState::btnOkClick(Action *action)
	{
	  _game->popState();
	}
	void EndResearchState::btnReportClick(Action *action)
	{
	}

}
