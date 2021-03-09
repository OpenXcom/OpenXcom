#include "JanetAIModule.h"

namespace OpenXcom
{
  JanetAIModule::JanetAIModule(SavedBattleGame *save, BattleUnit *unit, Node *node) : AIModule(save, unit, node)
  {
    // FIXME: Let's make the Janet interpreter a class-level variable.
    // Initialize it... sometime. Not sure.
  }
}
