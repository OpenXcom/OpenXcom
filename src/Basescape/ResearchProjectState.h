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

class ResearchProjectState : public State
{
private:
	Base *_base;
	TextButton *_btnOk;
	ImageButton * _btnMore, *_btnLess, *btn1,*btn2,*btn3,*btn4;
	Window *_window;
	Text *_txtTitle, *_txtAvailableScientist, *_txtAvailableSpace, *_txtAssigned, *_txtMore, *_txtLess;
	TextList *_lstResearch;
	std::vector<Text*> texts_;
	void SetAssignedScientist(int nb);
public:
	/// Creates the Research state.
	ResearchProjectState(Game *game, Base *base);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	void btnMoreClick(Action *action);
	void btnLessClick(Action *action);
};
}

#endif
