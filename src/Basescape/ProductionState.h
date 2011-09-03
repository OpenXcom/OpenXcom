#ifndef PRODUCTION_STATE_H
#define PRODUCTION_STATE_H

#include "../Engine/State.h"

namespace OpenXcom
{
class Base;
class Window;
class Text;
class ArrowButton;
class TextButton;
class RuleItem;
class Production;
class Timer;

class ProductionState : public State
{
private:
	Production * _production;
	RuleItem * _item;
	Base * _base;
	Window * _window;
	ArrowButton * _btnUnitUp, * _btnUnitDown, * _btnEngineerUp, * _btnEngineerDown;
	TextButton * _btnStop, * _btnOk;
	Text * _txtTitle, * _txtAvailableEngineer, * _txtAvailableSpace, * _txtAllocatedEngineer, * _txtUnitToProduce, * _txtUnitUp, * _txtUnitDown, * _txtEngineerUp, * _txtEngineerDown, * _txtAllocated, * _txtTodo;
	Timer * _timerMoreEngineer, * _timerMoreUnit, * _timerLessEngineer, * _timerLessUnit;
	void btnStopClick (Action * action);
	void btnOkClick (Action * action);
	void moreEngineerPress(Action * action);
	void moreEngineerRelease(Action * action);
	void moreUnitPress(Action * action);
	void moreUnitRelease(Action * action);
	void lessEngineerPress(Action * action);
	void lessEngineerRelease(Action * action);
	void lessUnitPress(Action * action);
	void lessUnitRelease(Action * action);
	void onMoreEngineer(Action * action);
	void onLessEngineer(Action * action);
	void onMoreUnit(Action * action);
	void onLessUnit(Action * action);
	void setAssignedEngineer();
	void think();
public:
	ProductionState (Game * game, Base * base, RuleItem * _item);
};
}
#endif
