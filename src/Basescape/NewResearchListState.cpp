#include "NewResearchListState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Base.h"
#include "ResearchProjectState.h"
#include "../Ruleset/RuleResearchProject.h"
#include "../Ruleset/Ruleset.h"
#include "../Savegame/ResearchProject.h"
#include "../Savegame/ItemContainer.h"

#include <algorithm>

namespace OpenXcom
{

struct findRuleResearchProject : public std::unary_function<ResearchProject *,
							    bool>
{
	RuleResearchProject * _toFind;
	findRuleResearchProject(RuleResearchProject * toFind);
	bool operator()(ResearchProject *r) const;
};

findRuleResearchProject::findRuleResearchProject(RuleResearchProject * toFind) : _toFind(toFind)
{
}

bool findRuleResearchProject::operator()(ResearchProject *r) const
{
	return _toFind == r->getRuleResearchProject();
}

void getAvailableResearchProjects (std::vector<RuleResearchProject *> & projects, Game * game, Base * base)
{
	const std::vector<RuleResearchProject *> & researchProjects = game->getRuleset()->getResearchProjects();
	const std::vector<ResearchProject *> & baseResearchProjects = base->GetResearch();
	for(std::vector<RuleResearchProject *>::const_iterator iter = researchProjects.begin ();
	    iter != researchProjects.end ();
	    iter++)
	{
		if (!(*iter)->isAvailable () || (*iter)->isDiscovered () || std::find_if (baseResearchProjects.begin(), baseResearchProjects.end (), findRuleResearchProject(*iter)) != baseResearchProjects.end ())
			continue;
		if ((*iter)->needItem() && base->getItems()->getItem((*iter)->getName ()) == 0)
		  {
			continue;
		  }
		projects.push_back (*iter);
	}
}

NewResearchListState::NewResearchListState(Game *game, Base *base, ResearchState * researchState) : State(game), _base(base), _researchState(researchState)
{
	int width = 220;
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
	_btnCancel = new TextButton(width - 2 * button_x_border , button_height, start_x + button_x_border, start_y + height - button_height - button_y_border);
	_txtTitle = new Text(width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + button_y_border);
	_lstResearch = new TextList(width - 2 * button_x_border, height - 2 * button_height - 2 * button_y_border, start_x + button_x_border, start_y + button_y_border + button_height);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(1)), Palette::backPos, 16);

	add(_window);
	add(_btnCancel);
	add(_txtTitle);
	add(_lstResearch);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+13);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));
	
	_btnCancel->setColor(Palette::blockOffset(15)+9);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&NewResearchListState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_NEW_PROJECT"));

	_lstResearch->setColor(Palette::blockOffset(13));

	_lstResearch->setColumns(1, width - 2 * button_x_border);
	_lstResearch->setSelectable(true);
	_lstResearch->setBackground(_window);
	_lstResearch->setMargin(2);
	_lstResearch->setAlign(ALIGN_CENTER);
	_lstResearch->onMouseClick((ActionHandler)&NewResearchListState::onSelectProject);
	fillProjectList ();
}

void NewResearchListState::onSelectProject(Action *action)
{
	_game->pushState(new ResearchProjectState(_game, _base, _projects[_lstResearch->getSelectedRow()], _researchState, this));
}
  
void NewResearchListState::btnCancelClick(Action *action)
{
	_game->popState();
}

void NewResearchListState::fillProjectList ()
{
	_projects.clear();
	_lstResearch->clearList();
	getAvailableResearchProjects(_projects, _game, _base);
	if(_projects.empty())
	{
		_game->popState();
		//FIXME : if no project can be listed when the dialog is created for the first time, this will exit to the base view
	}
	else
	{
		for (std::vector<RuleResearchProject *>::iterator it = _projects.begin ();
		     it != _projects.end ();
		     it++)
	  	{
			_lstResearch->addRow(1, _game->getLanguage()->getString((*it)->getName ()).c_str());
		}
	}
}
}
