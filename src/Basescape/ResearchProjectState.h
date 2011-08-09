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
class ImageButton;
class RuleResearchProject;
class ResearchProject;
class ResearchState;
class NewResearchListState;

class ResearchProjectState : public State
{
private:
	Base *_base;
	TextButton *_btnOk;
	ImageButton * _btnMore, *_btnLess, *btn1,*btn2,*btn3,*btn4;
	Window *_window;
	Text *_txtTitle, *_txtAvailableScientist, *_txtAvailableSpace, *_txtAssigned, *_txtMore, *_txtLess;
	TextList *_lstResearch;
	ResearchState * _researchState;
	NewResearchListState * _newResearchListState;
	std::vector<Text*> texts_;
	void SetAssignedScientist(int nb);
	ResearchProject * _project;
	RuleResearchProject * _rule;
	void buildUi ();
public:
	/// Creates the Research state.
	ResearchProjectState(Game *game, Base *base, RuleResearchProject * rule, ResearchState * researchState, NewResearchListState * newResearchListState);
	ResearchProjectState(Game *game, Base *base, ResearchProject * project, ResearchState * researchState, NewResearchListState * newResearchListState);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	void btnMoreClick(Action *action);
	void btnLessClick(Action *action);
};
}

#endif
