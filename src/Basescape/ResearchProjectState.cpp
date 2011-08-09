#include "ResearchProjectState.h"
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
#include "../Interface/ImageButton.h"
#include "../Engine/SurfaceSet.h"
#include "../Ruleset/RuleResearchProject.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Savegame/ResearchProject.h"
#include "../Savegame/BaseFacility.h"

#include <sstream>

namespace OpenXcom
{
int getFreeLabSpace (Base * base)
{
  int freeLabSpace = 0;
  std::vector<BaseFacility*> *const facilities (base->getFacilities());
  for (std::vector<BaseFacility*>::iterator itFacility = facilities->begin ();
       itFacility != facilities->end ();
       itFacility++)
    {
      freeLabSpace += (*itFacility)->getRules()->getLaboratories();
    }
  
  const std::vector<ResearchProject *> & researchs (base->GetResearch());
  for (std::vector<ResearchProject *>::const_iterator itResearch = researchs.begin ();
       itResearch != researchs.end ();
       itResearch++)
    {
      freeLabSpace -= (*itResearch)->GetAssigned ();
    }
       
  return freeLabSpace;
}

int getAvailableScientist (Base * base)
{
  int nbFreeScientist = base->getScientists();
  const std::vector<ResearchProject *> & researchs (base->GetResearch());
  for (std::vector<ResearchProject *>::const_iterator itResearch = researchs.begin ();
       itResearch != researchs.end ();
       itResearch++)
    {
      nbFreeScientist -= (*itResearch)->GetAssigned ();
    }
       
  return nbFreeScientist;
}

ResearchProjectState::ResearchProjectState(Game *game, Base *base, RuleResearchProject * rule) : State(game), _base(base), _project(new ResearchProject(_rule)), _rule(rule)
{
  buildUi ();
}

ResearchProjectState::ResearchProjectState(Game *game, Base *base, ResearchProject * project) : State(game), _base(base), _project(project), _rule(0)
{
  buildUi ();
}

void ResearchProjectState::buildUi ()
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
	_window = new Window(this, width, height, start_x, start_y/*, POPUP_BOTH*/);

	_txtTitle = new Text(width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + button_y_border);
	
	_txtAvailableScientist = new Text(width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + 3*button_y_border);
	_txtAvailableSpace = new Text(width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + 4*button_y_border);
	_txtAssigned = new Text(width - 2 * button_x_border, button_height, start_x + 2*button_x_border, start_y + 5*button_y_border);
	_txtMore = new Text(width - 3 * button_x_border, button_height, start_x + 2*button_x_border, start_y + 7*button_y_border);
	_txtLess = new Text(width - 3 * button_x_border, button_height, start_x + 2*button_x_border, start_y + 9*button_y_border);
	_btnOk = new TextButton(width - 2 * button_x_border , button_height, start_x + button_x_border, start_y + height - button_height - button_y_border);

	_btnMore = new ImageButton(2*button_x_border, button_height, start_x + 7*button_x_border, start_y + 7*button_y_border);
	_btnLess = new ImageButton(2*button_x_border, button_height, start_x + 7*button_x_border, start_y + 9*button_y_border);
	btn1 = new ImageButton(2*button_x_border, button_height, start_x + 9*button_x_border, start_y + 7*button_y_border);
	btn2 = new ImageButton(2*button_x_border, button_height, start_x + 9*button_x_border, start_y + 9*button_y_border);
	btn3 = new ImageButton(2*button_x_border, button_height, start_x + 11*button_x_border, start_y + 7*button_y_border);
	btn4 = new ImageButton(2*button_x_border, button_height, start_x + 11*button_x_border, start_y + 9*button_y_border);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(1)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtAvailableScientist);
	add(_txtAvailableSpace);
	add(_txtAssigned);
	add(_txtMore);
	add(_txtLess);
	add(_btnMore);
	add(_btnLess);
	add(btn1);
	add(btn2);
	add(btn3);
	add(btn4);
	
	// Set up objects
	Uint8 c = Palette::blockOffset(13)+8;
	_window->setColor(c);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));
	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ResearchProjectState::btnOkClick);

	_txtTitle->setSecondaryColor(c);
	_txtTitle->setText(_rule ? _rule->getName() : _project->GetRuleResearchProject ()->getName());

	_txtMore->setText(L"More");
	_txtLess->setText(L"Less");
	_btnMore->copy(_window);
	_btnLess->copy(_window);

	if (_rule)
	  SetAssignedScientist(0);
	else
	  SetAssignedScientist(_project->GetAssigned ());

	_btnMore->setColor(c);
	_btnLess->setColor(c);
	_btnMore->onMouseClick((ActionHandler)&ResearchProjectState::btnMoreClick);
	_btnLess->onMouseClick((ActionHandler)&ResearchProjectState::btnLessClick);
}

void ResearchProjectState::btnOkClick(Action *action)
{
	_game->popState();
}

void ResearchProjectState::SetAssignedScientist(int nb)
{
	std::wstringstream s1;
	int freeScientist = getAvailableScientist(_base);
	int freeSpaceLab = getFreeLabSpace(_base);
	s1 << L"AVAILABLE SCIENTIST>" << freeScientist - nb;
	std::wstringstream s2;
	s2 << L"FREE LAB SPACE>" << freeSpaceLab - nb;
	std::wstringstream s3;
	s3 << L"Assigned Scientist>" << nb;
	_txtAvailableScientist->setText(s1.str());
	_txtAvailableSpace->setText(s2.str());
	_txtAssigned->setText(s3.str());
}

void ResearchProjectState::btnMoreClick(Action *action)
{
	int assigned = _project->GetAssigned ();
	if ((getAvailableScientist(_base) - assigned) > 0 && (getFreeLabSpace(_base) - assigned) > 0)
	{
		_project->setAssigned(++assigned);
		SetAssignedScientist(assigned);
	}
}


void ResearchProjectState::btnLessClick(Action *action)
{
	int assigned = _project->GetAssigned ();
	if (assigned > 0)
	{
		_project->setAssigned(--assigned);
		SetAssignedScientist(assigned);
	}
}

}
