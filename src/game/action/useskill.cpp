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

#include "useskill.h"

#include "../../common/logutil.h"

#include "../game.h"
#include "../object/creature.h"
#include "../object/factory.h"
#include "../script/runner.h"
#include "../services.h"

using namespace std;

namespace reone {

namespace game {

void UseSkillAction::execute(shared_ptr<Action> self, Object &actor, float dt) {
    if (_skill == SkillType::Security) {
        if (!_object || _object->type() != ObjectType::Door) {
            warn("ActionExecutor: unsupported OpenLock object: " + to_string(_object->id()));
            complete();
            return;
        }

        auto door = static_pointer_cast<Door>(_object);
        auto creatureActor = _game.objectFactory().getObjectById<Creature>(actor.id());

        bool reached = creatureActor->navigateTo(door->position(), true, kDefaultMaxObjectDistance, dt);
        if (reached) {
            creatureActor->face(*door);
            creatureActor->playAnimation(AnimationType::LoopingUnlockDoor);

            if (!door->isKeyRequired()) {
                door->setLocked(false);
                door->open(creatureActor);

                string onOpen(door->getOnOpen());
                if (!onOpen.empty()) {
                    _game.scriptRunner().run(onOpen, door->id(), actor.id());
                }
            }

            complete();
        }
    } else {
        complete();
    }
}

} // namespace game

} // namespace reone
