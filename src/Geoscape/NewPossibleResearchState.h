#ifndef OPENXCOM_NEWPOSSIBLERESEARCHSTATE
#define OPENXCOM_NEWPOSSIBLERESEARCHSTATE

#include "../Engine/State.h"
namespace OpenXcom
{
class Game;
class Window;
class TextButton;
class Text;
class Base;
class TextList;
class RuleResearchProject;

class NewPossibleResearchState : public State
{
	Window *_window;
	Text *_txtTitle;
	TextList * _possibilities;
     	TextButton *_btnSelect, *_btnOk;
	Base * _base;
public:
	NewPossibleResearchState(Game * game, Base * base, const std::vector<RuleResearchProject *> & possibilities);
	void btnOkClick(Action *action);
	void btnSelectClick(Action *action);
};
}
#endif
