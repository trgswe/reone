/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "usefeat.h"

#include "../combat.h"
#include "../game.h"
#include "../object.h"
#include "../object/factory.h"
#include "../services.h"

using namespace std;

namespace reone {

namespace game {

void UseFeatAction::execute(shared_ptr<Action> self, Object &actor, float dt) {
    if (_object->isDead()) {
        complete();
        return;
    }

    auto creatureActor = _game.objectFactory().getObjectById<Creature>(actor.id());

    // Make the actor follow its target. When reached, register an attack
    if (creatureActor->navigateTo(_object->position(), true, _range, dt)) {
        _game.combat().addAttack(move(creatureActor), _object, static_pointer_cast<ObjectAction>(self));
    }
}

} // namespace game

} // namespace reone
