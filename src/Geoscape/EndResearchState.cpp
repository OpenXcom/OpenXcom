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
EndResearchState::EndResearchState(Game * game, Base * base) : State (game), _base(base)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 210, 140, 70, 30, POPUP_BOTH);
	_btnOk = new TextButton(80, 16, 80, 145);
	_btnReport = new TextButton(80, 16, 190, 145);
	_txtTitle = new Text(200, 16, 80, 90);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnReport);
	add(_txtTitle);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+8);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnReport->setColor(Palette::blockOffset(8)+8);
	_btnReport->setText(_game->getLanguage()->getString("STR_VIEW_REPORTS"));

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_RESEARCH_COMPLETED"));
}

void EndResearchState::btnOkClick(Action *action)
{
	  _game->popState();
}

void EndResearchState::btnReportClick(Action *action)
{
}

}
