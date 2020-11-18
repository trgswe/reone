/*
 * Copyright (c) 2020 The reone project contributors
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

#include <memory>
#include <set>

#include "glm/vec3.hpp"

#include "../scene/modelscenenode.h"
#include "../render/walkmesh.h"

namespace reone {

namespace game {

class SpatialObject;

class Room {
public:
    Room(
        const std::string &name,
        const glm::vec3 &position,
        const std::shared_ptr<scene::ModelSceneNode> &model,
        const std::shared_ptr<render::Walkmesh> &walkmesh);

    void addTenant(SpatialObject *object);
    void removeTenant(SpatialObject *object);
    void update(float dt);

    const std::string &name() const;
    const glm::vec3 &position() const;
    std::shared_ptr<scene::ModelSceneNode> model() const;
    const render::Walkmesh *walkmesh() const;
    bool visible() const;

    void setVisible(bool visible);

private:
    std::string _name;
    glm::vec3 _position { 0.0f };
    std::shared_ptr<scene::ModelSceneNode> _model;
    std::shared_ptr<render::Walkmesh> _walkmesh;
    std::set<SpatialObject *> _tenants;
    bool _visible { true };
};

} // namespace game

} // namespace reone
