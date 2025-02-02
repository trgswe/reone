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

#pragma once

#include "../action.h"
#include "../object.h"
#include "../object/item.h"

namespace reone {

namespace game {

class GiveItemAction : public Action {
public:
    GiveItemAction(Game &game, Services &services, std::shared_ptr<Item> item, std::shared_ptr<Object> giveTo) :
        Action(game, services, ActionType::GiveItem),
        _item(std::move(item)),
        _giveTo(std::move(giveTo)) {
    }

    void execute(std::shared_ptr<Action> self, Object &actor, float dt) override;

private:
    std::shared_ptr<Item> _item;
    std::shared_ptr<Object> _giveTo;
};

} // namespace game

} // namespace reone
