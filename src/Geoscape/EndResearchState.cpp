#include "EndResearchState.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Ruleset/RuleResearchProject.h"
#include "NewPossibleResearchState.h"
#include "../Ufopaedia/Ufopaedia.h"
#include <algorithm>

namespace OpenXcom
{
extern void getAvailableResearchProjects (std::vector<RuleResearchProject *> & projects, Game * game, Base * base);
void getDependableResearch (std::vector<RuleResearchProject *> & dependables, const RuleResearchProject *research, Game * game, Base * base)
{
	std::vector<RuleResearchProject *> possibleProjects;
	getAvailableResearchProjects(possibleProjects, game, base);
	for(std::vector<RuleResearchProject *>::iterator iter = possibleProjects.begin ();
	    iter != possibleProjects.end ();
	    iter++)
	{
		if (std::find((*iter)->getDependencys().begin (), (*iter)->getDependencys().end (), research) != (*iter)->getDependencys().end ())
		{
			dependables.push_back(*iter);
		}
	}
}

EndResearchState::EndResearchState(Game * game, Base * base, const RuleResearchProject * research) : State (game), _base(base), _research(research)
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
	_btnOk->onMouseClick((ActionHandler)&EndResearchState::btnOkClick);
	_btnReport->setColor(Palette::blockOffset(8)+8);
	_btnReport->setText(_game->getLanguage()->getString("STR_VIEW_REPORTS"));
	_btnReport->onMouseClick((ActionHandler)&EndResearchState::btnReportClick);

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_RESEARCH_COMPLETED"));
}

void EndResearchState::btnOkClick(Action *action)
{
	std::vector<RuleResearchProject *> newPossibleResearch;
	getDependableResearch (newPossibleResearch, _research, _game, _base);
	if (newPossibleResearch.empty ())
	{
		_game->popState();
	}
	else
	{
		_game->pushState (new NewPossibleResearchState(_game, _base, newPossibleResearch));
	}
}

void EndResearchState::btnReportClick(Action *action)
{
	std::string name (_research->getName ());
	Ufopaedia::openArticle(_game, name);
}

}
