#include "JanetAIModule.h"

namespace OpenXcom
{
  JanetTable* JanetAIModule::janet_env;

  JanetAIModule::JanetAIModule(SavedBattleGame *save, BattleUnit *unit, Node *node) : AIModule(save, unit, node)
  {
    _targetFaction = FACTION_HOSTILE;
  }
}
