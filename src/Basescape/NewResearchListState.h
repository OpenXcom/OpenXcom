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

class NewResearchListState : public State
{
private:
	Base *_base;
	TextButton *_btnCancel;
	Window *_window;
	Text *_txtTitle;
	TextList *_lstResearch;
	void onSelectProject(Action *action);
	void FillProjectList ();
	std::vector<RuleResearchProject *> _projects;
public:
	/// Creates the Research state.
	NewResearchListState(Game *game, Base *base);
	void btnCancelClick(Action *action);
};
}

#endif
