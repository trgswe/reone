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

#include "map.h"

#include "../../common/logutil.h"
#include "../../game/game.h"
#include "../../game/services.h"
#include "../../graphics/context.h"
#include "../../graphics/meshes.h"
#include "../../graphics/shaders.h"
#include "../../graphics/textures.h"
#include "../../graphics/uniforms.h"
#include "../../graphics/window.h"
#include "../../resource/gffstruct.h"

using namespace std;

using namespace reone::game;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace kotor {

static constexpr int kArrowSize = 32;
static constexpr int kMapNoteSize = 16;
static constexpr float kSelectedMapNoteScale = 1.5f;

void Map::load(const string &area, const GffStruct &gffs) {
    loadProperties(gffs);
    loadTextures(area);
}

void Map::loadProperties(const GffStruct &gffs) {
    _northAxis = gffs.getInt("NorthAxis");
    _worldPoint1 = glm::vec2(gffs.getFloat("WorldPt1X"), gffs.getFloat("WorldPt1Y"));
    _worldPoint2 = glm::vec2(gffs.getFloat("WorldPt2X"), gffs.getFloat("WorldPt2Y"));
    _mapPoint1 = glm::vec2(gffs.getFloat("MapPt1X"), gffs.getFloat("MapPt1Y"));
    _mapPoint2 = glm::vec2(gffs.getFloat("MapPt2X"), gffs.getFloat("MapPt2Y"));
}

void Map::loadTextures(const string &area) {
    string resRef("lbl_map" + area);
    _areaTexture = _services.textures.get(resRef, TextureUsage::GUI);

    if (!_arrowTexture) {
        _arrowTexture = _services.textures.get(_arrowResRef, TextureUsage::GUI);
    }
    if (!_noteTexture) {
        _noteTexture = _services.textures.get("whitetarget", TextureUsage::GUI);
    }
}

void Map::draw(Mode mode, const glm::vec4 &bounds) {
    if (!_areaTexture) {
        return;
    }
    _services.graphicsContext.withBlending(BlendMode::Normal, [this, &mode, &bounds]() {
        drawArea(mode, bounds);
        drawNotes(mode, bounds);
        drawPartyLeader(mode, bounds);
    });
}

void Map::drawArea(Mode mode, const glm::vec4 &bounds) {
    if (mode == Mode::Minimap) {
        shared_ptr<Creature> partyLeader(_game.party().getLeader());
        if (!partyLeader) {
            return;
        }
        _services.textures.bind(*_areaTexture);

        glm::vec2 worldPos(partyLeader->position());
        glm::vec2 mapPos(getMapPosition(worldPos));

        glm::vec3 topLeft(0.0f);
        topLeft.x = bounds[0] + 0.5f * bounds[2] - mapPos.x * 440.0f / static_cast<float>(_areaTexture->width()) * _areaTexture->width();
        topLeft.y = bounds[1] + 0.5f * bounds[3] - mapPos.y * _areaTexture->height();

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, topLeft);
        transform = glm::scale(transform, glm::vec3(_areaTexture->width(), _areaTexture->height(), 1.0f));

        _services.uniforms.setGeneral([this, transform](auto &general) {
            general.resetLocals();
            general.projection = _services.window.getOrthoProjection();
            general.model = move(transform);
        });
        _services.shaders.use(_services.shaders.gui());

        int height = _game.options().graphics.height;
        glm::ivec4 scissorBounds(bounds[0], height - (bounds[1] + bounds[3]), bounds[2], bounds[3]);
        _services.graphicsContext.withScissorTest(scissorBounds, [&]() {
            _services.meshes.quad().draw();
        });

    } else {
        _services.textures.bind(*_areaTexture);

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(bounds[0], bounds[1], 0.0f));
        transform = glm::scale(transform, glm::vec3(bounds[2], bounds[3], 1.0f));

        _services.uniforms.setGeneral([this, transform](auto &general) {
            general.resetLocals();
            general.projection = _services.window.getOrthoProjection();
            general.model = move(transform);
        });
        _services.shaders.use(_services.shaders.gui());
        _services.meshes.quad().draw();
    }
}

void Map::drawNotes(Mode mode, const glm::vec4 &bounds) {
    if (mode != Mode::Default) {
        return;
    }
    _services.textures.bind(*_noteTexture);

    for (auto &object : _game.module()->area()->getObjectsByType(ObjectType::Waypoint)) {
        auto waypoint = static_pointer_cast<Waypoint>(object);
        if (!waypoint->isMapNoteEnabled() || waypoint->mapNote().empty())
            continue;

        glm::vec2 mapPos(getMapPosition(waypoint->position()));
        mapPos.x *= bounds[2] / static_cast<float>(_areaTexture->width());
        mapPos.y *= bounds[3] / static_cast<float>(_areaTexture->height());

        glm::vec2 notePos;
        notePos.x = bounds[0] + mapPos.x * bounds[2];
        notePos.y = bounds[1] + mapPos.y * bounds[3];

        bool selected = waypoint == _selectedNote;
        float noteSize = (selected ? kSelectedMapNoteScale : 1.0f) * kMapNoteSize;

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(notePos.x - 0.5f * noteSize, notePos.y - 0.5f * noteSize, 0.0f));
        transform = glm::scale(transform, glm::vec3(noteSize, noteSize, 1.0f));

        _services.uniforms.setGeneral([this, transform, &selected](auto &general) {
            general.resetLocals();
            general.projection = _services.window.getOrthoProjection();
            general.model = move(transform);
            general.color = glm::vec4(selected ? _game.getGUIColorHilight() : _game.getGUIColorBase(), 1.0f);
        });
        _services.shaders.use(_services.shaders.gui());
        _services.meshes.quad().draw();
    }
}

glm::vec2 Map::getMapPosition(const glm::vec2 &world) const {
    float scaleX, scaleY;
    glm::vec2 result(0.0f);

    switch (_northAxis) {
    case 0:
    case 1:
        scaleX = (_mapPoint1.x - _mapPoint2.x) / (_worldPoint1.x - _worldPoint2.x);
        scaleY = (_mapPoint1.y - _mapPoint2.y) / (_worldPoint1.y - _worldPoint2.y);
        result.x = (world.x - _worldPoint1.x) * scaleX + _mapPoint1.x;
        result.y = (world.y - _worldPoint1.y) * scaleY + _mapPoint1.y;
        break;
    case 2:
    case 3:
        scaleX = (_mapPoint1.y - _mapPoint2.y) / (_worldPoint1.x - _worldPoint2.x);
        scaleY = (_mapPoint1.x - _mapPoint2.x) / (_worldPoint1.y - _worldPoint2.y);
        result.x = (world.y - _worldPoint1.y) * scaleY + _mapPoint1.x;
        result.y = (world.x - _worldPoint1.x) * scaleX + _mapPoint1.y;
        break;
    default:
        warn("Map: invalid north axis: " + to_string(_northAxis));
        break;
    }

    return move(result);
}

void Map::drawPartyLeader(Mode mode, const glm::vec4 &bounds) {
    shared_ptr<Creature> partyLeader(_game.party().getLeader());
    if (!partyLeader) {
        return;
    }
    _services.textures.bind(*_arrowTexture);

    glm::vec3 arrowPos(0.0f);

    if (mode == Mode::Default) {
        glm::vec2 worldPos(partyLeader->position());
        glm::vec2 mapPos(getMapPosition(worldPos));

        mapPos.x *= bounds[2] / static_cast<float>(_areaTexture->width());
        mapPos.y *= bounds[3] / static_cast<float>(_areaTexture->height());

        arrowPos.x = bounds[0] + mapPos.x * bounds[2];
        arrowPos.y = bounds[1] + mapPos.y * bounds[3];

    } else {
        arrowPos.x = bounds[0] + 0.5f * bounds[2];
        arrowPos.y = bounds[1] + 0.5f * bounds[3];
    }

    float facing;
    switch (_northAxis) {
    case 0:
        facing = -partyLeader->getFacing();
        break;
    case 1:
        facing = glm::pi<float>() - partyLeader->getFacing();
        break;
    case 2:
        facing = glm::three_over_two_pi<float>() - partyLeader->getFacing();
        break;
    default:
        facing = glm::half_pi<float>() - partyLeader->getFacing();
        break;
    }
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, arrowPos);
    transform = glm::rotate(transform, facing, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, glm::vec3(-0.5f * kArrowSize, -0.5f * kArrowSize, 0.0f));
    transform = glm::scale(transform, glm::vec3(kArrowSize, kArrowSize, 1.0f));

    _services.uniforms.setGeneral([this, transform](auto &general) {
        general.resetLocals();
        general.projection = _services.window.getOrthoProjection();
        general.model = move(transform);
    });
    _services.shaders.use(_services.shaders.gui());
    _services.meshes.quad().draw();
}

} // namespace kotor

} // namespace reone
