#ifndef OPENXCOM_RESEARCHPROJECTSTATE
#define OPENXCOM_RESEARCHPROJECTSTATE
#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;
class RuleResearchProject;
class ResearchProject;
class ResearchState;
class NewResearchListState;
class ArrowButton;
class Timer;

class ResearchProjectState : public State
{
private:
	Base *_base;
	TextButton *_btnOk;
	ArrowButton * _btnMore, *_btnLess;
	Window *_window;
	Text *_txtTitle, *_txtAvailableScientist, *_txtAvailableSpace, *_txtAssigned, *_txtMore, *_txtLess;
	TextList *_lstResearch;
	ResearchState * _researchState;
	NewResearchListState * _newResearchListState;
	std::vector<Text*> texts_;
	void SetAssignedScientist();
	ResearchProject * _project;
	RuleResearchProject * _rule;
	void buildUi ();
	Timer *_timerMore, *_timerLess;
public:
	/// Creates the Research state.
	ResearchProjectState(Game *game, Base *base, RuleResearchProject * rule, ResearchState * researchState, NewResearchListState * newResearchListState);
	ResearchProjectState(Game *game, Base *base, ResearchProject * project, ResearchState * researchState, NewResearchListState * newResearchListState);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	void btnMoreClick(Action *action);
	void btnLessClick(Action *action);
	void more();
	void less();
	void morePress(Action *action);
	void moreRelease(Action *action);
	void lessPress(Action *action);
	void lessRelease(Action *action);
	void think();
};
}

#endif
