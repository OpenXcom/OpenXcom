#ifndef OPENXCOM_ENDRESEARCHSTATE
#define OPENXCOM_ENDRESEARCHSTATE

#include "../Engine/State.h"
namespace OpenXcom
{
class Game;
class Window;
class TextButton;
class Text;
class Base;
class RuleResearchProject;

class EndResearchState : public State
{
	Window *_window;
	Text *_txtTitle;
     	TextButton *_btnReport, *_btnOk;
	Base * _base;
	const RuleResearchProject * _research;
public:
	EndResearchState(Game * game, Base * base, const RuleResearchProject * research);
	void btnOkClick(Action *action);
	void btnReportClick(Action *action);
};
}
#endif
