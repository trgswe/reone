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

#include "module.h"

#include "../../common/exception/validation.h"
#include "../../common/logutil.h"
#include "../../resource/gffs.h"
#include "../../resource/resources.h"

#include "../action/attack.h"
#include "../action/factory.h"
#include "../action/opencontainer.h"
#include "../action/opendoor.h"
#include "../action/startconversation.h"
#include "../game.h"
#include "../party.h"
#include "../reputes.h"
#include "../services.h"

#include "factory.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr int kMaxMillisecond = 1000;
static constexpr int kMaxSecond = 60;
static constexpr int kMaxMinute = 60;
static constexpr int kMaxHour = 24;

void Module::load(string name, const GffStruct &ifo, bool fromSave) {
    _name = move(name);

    loadInfo(ifo);
    loadArea(ifo);

    _area->initCameras(_info.entryPosition, _info.entryFacing);

    loadPlayer();

    if (!fromSave) {
        _area->runSpawnScripts();
    }
}

void Module::loadInfo(const GffStruct &ifo) {
    // Entry location

    _info.entryArea = ifo.getString("Mod_Entry_Area");
    if (_info.entryArea.empty()) {
        throw ValidationException("Mod_Entry_Area must not be empty");
    }

    _info.entryPosition.x = ifo.getFloat("Mod_Entry_X");
    _info.entryPosition.y = ifo.getFloat("Mod_Entry_Y");
    _info.entryPosition.z = ifo.getFloat("Mod_Entry_Z");

    float dirX = ifo.getFloat("Mod_Entry_Dir_X");
    float dirY = ifo.getFloat("Mod_Entry_Dir_Y");
    _info.entryFacing = -glm::atan(dirX, dirY);

    // Time

    _info.dawnHour = ifo.getInt("Mod_DawnHour");
    _info.duskHour = ifo.getInt("Mod_DuskHour");
    _info.minPerHour = ifo.getInt("Mod_MinPerHour");

    _time.hour = ifo.getInt("Mod_StartHour");
}

void Module::loadArea(const GffStruct &ifo, bool fromSave) {
    reone::info("Load area '" + _info.entryArea + "'");

    _area = _game.objectFactory().newArea();

    shared_ptr<GffStruct> are(_services.gffs.get(_info.entryArea, ResourceType::Are));
    if (!are) {
        throw ValidationException("Area ARE file not found");
    }

    shared_ptr<GffStruct> git(_services.gffs.get(_info.entryArea, ResourceType::Git));
    if (!git) {
        throw ValidationException("Area GIT file not found");
    }

    _area->load(_info.entryArea, *are, *git, fromSave);
}

void Module::loadPlayer() {
    _player = make_unique<Player>(*this, *_area, _area->getCamera(CameraType::ThirdPerson), _game.party());
}

void Module::loadParty(const string &entry, bool fromSave) {
    glm::vec3 position(0.0f);
    float facing = 0.0f;
    getEntryPoint(entry, position, facing);

    _area->loadParty(position, facing, fromSave);
    _area->onPartyLeaderMoved(true);
    _area->update3rdPersonCameraFacing();

    if (!fromSave) {
        _area->runOnEnterScript();
    }
}

void Module::getEntryPoint(const string &waypoint, glm::vec3 &position, float &facing) const {
    position = _info.entryPosition;
    facing = _info.entryFacing;

    if (!waypoint.empty()) {
        shared_ptr<Object> object(_area->getObjectByTag(waypoint));
        if (object) {
            position = object->position();
            facing = object->getFacing();
        }
    }
}

bool Module::handle(const SDL_Event &event) {
    if (_player && _player->handle(event))
        return true;
    if (_area->handle(event))
        return true;

    switch (event.type) {
    case SDL_MOUSEMOTION:
        if (handleMouseMotion(event.motion))
            return true;
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (handleMouseButtonDown(event.button))
            return true;
        break;
    case SDL_KEYDOWN:
        if (handleKeyDown(event.key))
            return true;
        break;
    default:
        break;
    }

    return false;
}

bool Module::handleMouseMotion(const SDL_MouseMotionEvent &event) {
    CursorType cursor = CursorType::Default;

    auto object = _area->getObjectAt(event.x, event.y);
    if (object && object->isSelectable()) {
        auto objectPtr = _game.objectFactory().getObjectById(object->id());
        _area->hilightObject(objectPtr);

        switch (object->type()) {
        case ObjectType::Creature: {
            if (object->isDead()) {
                cursor = CursorType::Pickup;
            } else {
                auto creature = static_cast<Creature *>(object);
                bool isEnemy = _services.reputes.getIsEnemy(*creature, *_game.party().getLeader());
                cursor = isEnemy ? CursorType::Attack : CursorType::Talk;
            }
            break;
        }
        case ObjectType::Door:
            cursor = CursorType::Door;
            break;
        case ObjectType::Placeable:
            cursor = CursorType::Pickup;
            break;
        default:
            break;
        }
    } else {
        _area->hilightObject(nullptr);
    }

    _game.setCursorType(cursor);

    return true;
}

bool Module::handleMouseButtonDown(const SDL_MouseButtonEvent &event) {
    if (event.button != SDL_BUTTON_LEFT) {
        return false;
    }
    auto object = _area->getObjectAt(event.x, event.y);
    if (!object || !object->isSelectable()) {
        return false;
    }
    auto objectPtr = _game.objectFactory().getObjectById(object->id());
    auto selectedObject = _area->selectedObject();
    if (objectPtr != selectedObject) {
        _area->selectObject(objectPtr);
        return true;
    }
    onObjectClick(objectPtr);

    return true;
}

void Module::onObjectClick(const shared_ptr<Object> &object) {
    switch (object->type()) {
    case ObjectType::Creature:
        onCreatureClick(static_pointer_cast<Creature>(object));
        break;
    case ObjectType::Door:
        onDoorClick(static_pointer_cast<Door>(object));
        break;
    case ObjectType::Placeable:
        onPlaceableClick(static_pointer_cast<Placeable>(object));
        break;
    default:
        break;
    }
}

void Module::onCreatureClick(const shared_ptr<Creature> &creature) {
    debug(boost::format("Module: click: creature '%s', faction %d") % creature->tag() % static_cast<int>(creature->faction()));

    shared_ptr<Creature> partyLeader(_game.party().getLeader());

    if (creature->isDead()) {
        if (!creature->items().empty()) {
            partyLeader->clearAllActions();
            partyLeader->addAction(_game.actionFactory().newOpenContainer(creature));
        }
    } else {
        bool isEnemy = _services.reputes.getIsEnemy(*partyLeader, *creature);
        if (isEnemy) {
            partyLeader->clearAllActions();
            partyLeader->addAction(_game.actionFactory().newAttack(creature));
        } else if (!creature->conversation().empty()) {
            partyLeader->clearAllActions();
            partyLeader->addAction(_game.actionFactory().newStartConversation(creature, creature->conversation()));
        }
    }
}

void Module::onDoorClick(const shared_ptr<Door> &door) {
    if (!door->linkedToModule().empty()) {
        _game.scheduleModuleTransition(door->linkedToModule(), door->linkedTo());
        return;
    }
    if (!door->isOpen()) {
        shared_ptr<Creature> partyLeader(_game.party().getLeader());
        partyLeader->clearAllActions();
        partyLeader->addAction(_game.actionFactory().newOpenDoor(door));
    }
}

void Module::onPlaceableClick(const shared_ptr<Placeable> &placeable) {
    shared_ptr<Creature> partyLeader(_game.party().getLeader());

    if (placeable->hasInventory()) {
        partyLeader->clearAllActions();
        partyLeader->addAction(_game.actionFactory().newOpenContainer(placeable));
    } else if (!placeable->conversation().empty()) {
        partyLeader->clearAllActions();
        partyLeader->addAction(_game.actionFactory().newStartConversation(placeable, placeable->conversation()));
    } else {
        placeable->runOnUsed(move(partyLeader));
    }
}

void Module::update(float dt) {
    if (_game.cameraType() == CameraType::ThirdPerson) {
        _player->update(dt);
    }
    _area->update(dt);
}

vector<ContextAction> Module::getContextActions(const shared_ptr<Object> &object) const {
    vector<ContextAction> actions;

    switch (object->type()) {
    case ObjectType::Creature: {
        auto leader = _game.party().getLeader();
        auto creature = static_pointer_cast<Creature>(object);
        if (!creature->isDead() && _services.reputes.getIsEnemy(*leader, *creature)) {
            actions.push_back(ContextAction(ActionType::AttackObject));
            auto weapon = leader->getEquippedItem(InventorySlot::rightWeapon);
            if (weapon && weapon->isRanged()) {
                if (leader->attributes().hasFeat(FeatType::MasterPowerBlast)) {
                    actions.push_back(ContextAction(FeatType::MasterPowerBlast));
                } else if (leader->attributes().hasFeat(FeatType::ImprovedPowerBlast)) {
                    actions.push_back(ContextAction(FeatType::ImprovedPowerBlast));
                } else if (leader->attributes().hasFeat(FeatType::PowerBlast)) {
                    actions.push_back(ContextAction(FeatType::PowerBlast));
                }
                if (leader->attributes().hasFeat(FeatType::MasterSniperShot)) {
                    actions.push_back(ContextAction(FeatType::MasterSniperShot));
                } else if (leader->attributes().hasFeat(FeatType::ImprovedSniperShot)) {
                    actions.push_back(ContextAction(FeatType::ImprovedSniperShot));
                } else if (leader->attributes().hasFeat(FeatType::SniperShot)) {
                    actions.push_back(ContextAction(FeatType::SniperShot));
                }
                if (leader->attributes().hasFeat(FeatType::MultiShot)) {
                    actions.push_back(ContextAction(FeatType::MultiShot));
                } else if (leader->attributes().hasFeat(FeatType::ImprovedRapidShot)) {
                    actions.push_back(ContextAction(FeatType::ImprovedRapidShot));
                } else if (leader->attributes().hasFeat(FeatType::RapidShot)) {
                    actions.push_back(ContextAction(FeatType::RapidShot));
                }
            } else {
                if (leader->attributes().hasFeat(FeatType::MasterPowerAttack)) {
                    actions.push_back(ContextAction(FeatType::MasterPowerAttack));
                } else if (leader->attributes().hasFeat(FeatType::ImprovedPowerAttack)) {
                    actions.push_back(ContextAction(FeatType::ImprovedPowerAttack));
                } else if (leader->attributes().hasFeat(FeatType::PowerAttack)) {
                    actions.push_back(ContextAction(FeatType::PowerAttack));
                }
                if (leader->attributes().hasFeat(FeatType::MasterCriticalStrike)) {
                    actions.push_back(ContextAction(FeatType::MasterCriticalStrike));
                } else if (leader->attributes().hasFeat(FeatType::ImprovedCriticalStrike)) {
                    actions.push_back(ContextAction(FeatType::ImprovedCriticalStrike));
                } else if (leader->attributes().hasFeat(FeatType::CriticalStrike)) {
                    actions.push_back(ContextAction(FeatType::CriticalStrike));
                }
                if (leader->attributes().hasFeat(FeatType::WhirlwindAttack)) {
                    actions.push_back(ContextAction(FeatType::WhirlwindAttack));
                } else if (leader->attributes().hasFeat(FeatType::ImprovedFlurry)) {
                    actions.push_back(ContextAction(FeatType::ImprovedFlurry));
                } else if (leader->attributes().hasFeat(FeatType::Flurry)) {
                    actions.push_back(ContextAction(FeatType::Flurry));
                }
            }
        }
        break;
    }
    case ObjectType::Door: {
        auto door = static_pointer_cast<Door>(object);
        if (door->isLocked() && !door->isKeyRequired() && _game.party().getLeader()->attributes().hasSkill(SkillType::Security)) {
            actions.push_back(ContextAction(SkillType::Security));
        }
        break;
    }
    default:
        break;
    }

    return move(actions);
}

bool Module::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.sym) {
    case SDLK_SPACE: {
        bool paused = !_game.isPaused();
        _game.setPaused(paused);
        return true;
    }
    default:
        return false;
    }
}

void Module::setTime(int hour, int minute, int second, int millisecond) {
    if (millisecond <= _time.millisecond) {
        _time.millisecond = millisecond;
    } else {
        _time.millisecond = millisecond % kMaxMillisecond;
        second += millisecond / kMaxMillisecond;
    }
    if (second <= _time.second) {
        _time.second = second;
    } else {
        _time.second = second % kMaxSecond;
        minute += second / kMaxSecond;
    }
    if (minute <= _time.minute) {
        _time.minute = minute;
    } else {
        _time.minute = minute % kMaxMinute;
        hour += minute / kMaxMinute;
    }
    if (hour <= _time.hour) {
        _time.hour = hour;
    } else {
        _time.hour = hour % kMaxHour;
        _time.day += hour / kMaxHour;
    }
}

} // namespace game

} // namespace reone
