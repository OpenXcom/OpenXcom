#ifndef OPENXCOM_NEWRESEARCHLISTSTATE_H
#define OPENXCOM_NEWRESEARCHLISTSTATE_H
#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;
class RuleResearchProject;
class ResearchState;

class NewResearchListState : public State
{
private:
	Base *_base;
	TextButton *_btnCancel;
	Window *_window;
	Text *_txtTitle;
	TextList *_lstResearch;
	void onSelectProject(Action *action);
	std::vector<RuleResearchProject *> _projects;
	ResearchState * _researchState;
public:
	/// Creates the Research state.
	NewResearchListState(Game *game, Base *base, ResearchState * researchState);
	void btnCancelClick(Action *action);
	void FillProjectList ();
};
}

#endif
