#ifndef OPENXCOM_ENDRESEARCHSTATE
#define OPENXCOM_ENDRESEARCHSTATE

#include "../Engine/State.h"
namespace OpenXcom
{
  class Game;
  class Window;
  class TextButton;
  class Text;

  class EndResearchState : public State
  {
	TextButton *_btnOk, *_btnOk5Secs;
	Window *_window;
	Text *_txtTitle;
    // 	TextButton *_btnReport, *_btnOk;
    // 	Window *_window;
    // Text * _txtEndResearch;
  public:
    EndResearchState(Game * game);
	void btnOkClick(Action *action);
	void btnReportClick(Action *action);
  };
}
#endif
