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

#include "room.h"

#include "../../scene/node/model.h"

#include "object.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::scene;

namespace reone {

namespace game {

void Room::addTenant(Object *object) {
    _tenants.insert(object);
}

void Room::removeTenant(Object *object) {
    _tenants.erase(object);
}

void Room::setVisible(bool visible) {
    for (auto &tenant : _tenants) {
        tenant->setVisible(visible);
    }
    if (_visible == visible) {
        return;
    }
    if (_model) {
        _model->setEnabled(visible);
    }
    if (_walkmesh) {
        _walkmesh->setEnabled(visible);
    }
    if (_grass) {
        _grass->setEnabled(visible);
    }
    _visible = visible;
}

} // namespace game

} // namespace reone
