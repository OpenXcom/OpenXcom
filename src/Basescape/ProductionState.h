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

/**
 * Screen which allow to change Production settings(assigned engineer, units to build).
*/
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
	/// Handler for the Stop button
	void btnStopClick (Action * action);
	/// Handler for the OK button
	void btnOkClick (Action * action);
	/// Handler for pressing the more engineer button
	void moreEngineerPress(Action * action);
	/// Handler for releasing the more engineer button
	void moreEngineerRelease(Action * action);
	/// Handler for pressing the more unit button
	void moreUnitPress(Action * action);
	/// Handler for releasing the more unit button
	void moreUnitRelease(Action * action);
	/// Handler for pressing the less engineer button
	void lessEngineerPress(Action * action);
	/// Handler for releasing the less engineer button
	void lessEngineerRelease(Action * action);
	/// Handler for pressing the less unit button 
	void lessUnitPress(Action * action);
	/// Handler for releasing the less unit button
	void lessUnitRelease(Action * action);
	/// Add one engineer to production (if possible)
	void onMoreEngineer(Action * action);
	/// Remove one engineer to production (if possible)
	void onLessEngineer(Action * action);
	/// Increase count of number of unit to do
	void onMoreUnit(Action * action);
	/// Decrease count of number of unit to do(if possible)
	void onLessUnit(Action * action);
	/// Update display of assigned/available engineer, workshop space
	void setAssignedEngineer();
	/// Runs state functionality every cycle.
	void think();
	/// Build the User Interface
	void buildUi();
	/// Helper to exit the State
	void exitState();
public:
	/// Create the State(new production)
	ProductionState (Game * game, Base * base, RuleItem * _item);
	/// Create the State(modify production)
	ProductionState (Game * game, Base * base, Production * production);
};
}
#endif
