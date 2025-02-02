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

#include "attack.h"
#include "barkstring.h"
#include "castfakespellatlocation.h"
#include "castfakespellatobject.h"
#include "castspellatobject.h"
#include "closedoor.h"
#include "docommand.h"
#include "equipitem.h"
#include "equipmostdamagingmelee.h"
#include "equipmostdamagingranged.h"
#include "follow.h"
#include "followleader.h"
#include "followowner.h"
#include "giveitem.h"
#include "interactobject.h"
#include "jumptolocation.h"
#include "jumptoobject.h"
#include "lockobject.h"
#include "moveawayfromobject.h"
#include "movetolocation.h"
#include "movetoobject.h"
#include "movetopoint.h"
#include "opencontainer.h"
#include "opendoor.h"
#include "openlock.h"
#include "pauseconversation.h"
#include "playanimation.h"
#include "randomwalk.h"
#include "resumeconversation.h"
#include "startconversation.h"
#include "surrendertoenemies.h"
#include "switchweapons.h"
#include "takeitem.h"
#include "unequipitem.h"
#include "unlockobject.h"
#include "usefeat.h"
#include "useskill.h"
#include "usetalentonobject.h"
#include "wait.h"

namespace reone {

namespace game {

class Game;
struct Services;

class ActionFactory {
public:
    ActionFactory(Game &game, Services &services) :
        _game(game),
        _services(services) {
    }

    std::unique_ptr<AttackAction> newAttack(std::shared_ptr<Object> object, float range = 1.0f, bool userAction = false, bool passive = false) {
        return std::make_unique<AttackAction>(_game, _services, std::move(object), range, userAction, passive);
    }

    std::unique_ptr<BarkStringAction> newBarkString(int strRef) {
        return std::make_unique<BarkStringAction>(_game, _services, strRef);
    }

    std::unique_ptr<CastFakeSpellAtLocationAction> newCastFakeSpellAtLocation(SpellType spell, std::shared_ptr<Location> location, ProjectilePathType projectilePathType) {
        return std::make_unique<CastFakeSpellAtLocationAction>(_game, _services, spell, std::move(location), projectilePathType);
    }

    std::unique_ptr<CastFakeSpellAtObjectAction> newCastFakeSpellAtObject(SpellType spell, std::shared_ptr<Object> target, ProjectilePathType projectilePathType) {
        return std::make_unique<CastFakeSpellAtObjectAction>(_game, _services, spell, std::move(target), projectilePathType);
    }

    std::unique_ptr<CastSpellAtObjectAction> newCastSpellAtObject(SpellType spell, std::shared_ptr<Object> target, int metaMagic, bool cheat, int domainLevel, ProjectilePathType projectilePathType, bool instantSpell) {
        return std::make_unique<CastSpellAtObjectAction>(_game, _services, spell, std::move(target), metaMagic, cheat, domainLevel, projectilePathType, instantSpell);
    }

    std::unique_ptr<CloseDoorAction> newCloseDoor(std::shared_ptr<Object> object) {
        return std::make_unique<CloseDoorAction>(_game, _services, std::move(object));
    }

    std::unique_ptr<CommandAction> newDoCommand(std::shared_ptr<script::ExecutionContext> context) {
        return std::make_unique<CommandAction>(_game, _services, std::move(context));
    }

    std::unique_ptr<EquipItemAction> newEquipItem(std::shared_ptr<Item> item, int inventorySlot, bool instant) {
        return std::make_unique<EquipItemAction>(_game, _services, std::move(item), inventorySlot, instant);
    }

    std::unique_ptr<EquipMostDamagingMeleeAction> newEquipMostDamagingMelee(std::shared_ptr<Object> versus, bool offHand) {
        return std::make_unique<EquipMostDamagingMeleeAction>(_game, _services, std::move(versus), offHand);
    }

    std::unique_ptr<EquipMostDamagingRangedAction> newEquipMostDamagingRanged(std::shared_ptr<Object> versus) {
        return std::make_unique<EquipMostDamagingRangedAction>(_game, _services, std::move(versus));
    }

    std::unique_ptr<FollowAction> newFollow(std::shared_ptr<Object> object, float distance) {
        return std::make_unique<FollowAction>(_game, _services, std::move(object), distance);
    }

    std::unique_ptr<FollowLeaderAction> newFollowLeader() {
        return std::make_unique<FollowLeaderAction>(_game, _services);
    }

    std::unique_ptr<FollowOwnerAction> newFollowOwner(float range) {
        return std::make_unique<FollowOwnerAction>(_game, _services, range);
    }

    std::unique_ptr<GiveItemAction> newGiveItem(std::shared_ptr<Item> item, std::shared_ptr<Object> giveTo) {
        return std::make_unique<GiveItemAction>(_game, _services, std::move(item), std::move(giveTo));
    }

    std::unique_ptr<InteractObjectAction> newInteractObject(std::shared_ptr<Placeable> placeable) {
        return std::make_unique<InteractObjectAction>(_game, _services, std::move(placeable));
    }

    std::unique_ptr<JumpToLocationAction> newJumpToLocation(std::shared_ptr<Location> location) {
        return std::make_unique<JumpToLocationAction>(_game, _services, std::move(location));
    }

    std::unique_ptr<JumpToObjectAction> newJumpToObject(std::shared_ptr<Object> object, bool walkStraightLine) {
        return std::make_unique<JumpToObjectAction>(_game, _services, std::move(object), walkStraightLine);
    }

    std::unique_ptr<LockObjectAction> newLockObject(std::shared_ptr<Object> target) {
        return std::make_unique<LockObjectAction>(_game, _services, std::move(target));
    }

    std::unique_ptr<MoveAwayFromObject> newMoveAwayFromObject(std::shared_ptr<Object> fleeFrom, bool run, float range) {
        return std::make_unique<MoveAwayFromObject>(_game, _services, std::move(fleeFrom), run, range);
    }

    std::unique_ptr<MoveToLocationAction> newMoveToLocation(std::shared_ptr<Location> destination, bool run = false, float timeout = 30.0f, bool force = false) {
        return std::make_unique<MoveToLocationAction>(_game, _services, std::move(destination), run, timeout, force);
    }

    std::unique_ptr<MoveToObjectAction> newMoveToObject(std::shared_ptr<Object> object, bool run, float range, float timeout = 30.0f, bool force = false) {
        return std::make_unique<MoveToObjectAction>(_game, _services, std::move(object), run, range, timeout, force);
    }

    std::unique_ptr<MoveToPointAction> newMoveToPoint(glm::vec3 point) {
        return std::make_unique<MoveToPointAction>(_game, _services, std::move(point));
    }

    std::unique_ptr<OpenContainerAction> newOpenContainer(std::shared_ptr<Object> object) {
        return std::make_unique<OpenContainerAction>(_game, _services, std::move(object));
    }

    std::unique_ptr<OpenDoorAction> newOpenDoor(std::shared_ptr<Object> object) {
        return std::make_unique<OpenDoorAction>(_game, _services, std::move(object));
    }

    std::unique_ptr<OpenLockAction> newOpenLock(std::shared_ptr<Object> object) {
        return std::make_unique<OpenLockAction>(_game, _services, std::move(object));
    }

    std::unique_ptr<PauseConversationAction> newPauseConversation() {
        return std::make_unique<PauseConversationAction>(_game, _services);
    }

    std::unique_ptr<PlayAnimationAction> newPlayAnimation(AnimationType anim, float speed = 1.0f, float durationSeconds = 0.0f) {
        return std::make_unique<PlayAnimationAction>(_game, _services, anim, speed, durationSeconds);
    }

    std::unique_ptr<RandomWalkAction> newRandomWalk() {
        return std::make_unique<RandomWalkAction>(_game, _services);
    }

    std::unique_ptr<ResumeConversationAction> newResumeConversation() {
        return std::make_unique<ResumeConversationAction>(_game, _services);
    }

    std::unique_ptr<StartConversationAction> newStartConversation(
        std::shared_ptr<Object> object,
        std::string dialogResRef,
        bool privateConversation = false,
        ConversationType conversationType = ConversationType::Cinematic,
        bool ignoreStartRange = false,
        std::string nameToIgnore1 = "",
        std::string nameToIgnore2 = "",
        std::string nameToIgnore3 = "",
        std::string nameToIgnore4 = "",
        std::string nameToIgnore5 = "",
        std::string nameToIgnore6 = "",
        bool useLeader = false,
        int barkX = -1,
        int barkY = -1,
        bool dontClearAllActions = false) {
        return std::make_unique<StartConversationAction>(
            _game,
            _services,
            std::move(object),
            std::move(dialogResRef),
            privateConversation,
            conversationType,
            ignoreStartRange,
            std::move(nameToIgnore1),
            std::move(nameToIgnore2),
            std::move(nameToIgnore3),
            std::move(nameToIgnore4),
            std::move(nameToIgnore5),
            std::move(nameToIgnore6),
            useLeader,
            barkX,
            barkY,
            dontClearAllActions);
    }

    std::unique_ptr<SurrenderToEnemiesAction> newSurrenderToEnemies() {
        return std::make_unique<SurrenderToEnemiesAction>(_game, _services);
    }

    std::unique_ptr<SwitchWeaponsAction> newSwitchWeapons() {
        return std::make_unique<SwitchWeaponsAction>(_game, _services);
    }

    std::unique_ptr<TakeItemAction> newTakeItem(std::shared_ptr<Item> item, std::shared_ptr<Object> takeFrom) {
        return std::make_unique<TakeItemAction>(_game, _services, move(item), move(takeFrom));
    }

    std::unique_ptr<UnequipItemAction> newUnequipItem(std::shared_ptr<Item> item, bool instant) {
        return std::make_unique<UnequipItemAction>(_game, _services, std::move(item), instant);
    }

    std::unique_ptr<UnlockObjectAction> newUnlockObject(std::shared_ptr<Object> target) {
        return std::make_unique<UnlockObjectAction>(_game, _services, std::move(target));
    }

    std::unique_ptr<UseFeatAction> newUseFeat(std::shared_ptr<Object> object, FeatType feat) {
        return std::make_unique<UseFeatAction>(_game, _services, std::move(object), feat);
    }

    std::unique_ptr<UseSkillAction> newUseSkill(std::shared_ptr<Object> object, SkillType skill, int subSkill = 0, std::shared_ptr<Item> itemUsed = nullptr) {
        return std::make_unique<UseSkillAction>(_game, _services, std::move(object), skill, subSkill, std::move(itemUsed));
    }

    std::unique_ptr<UseTalentOnObjectAction> newUseTalentOnObject(std::shared_ptr<Talent> chosenTalent, std::shared_ptr<Object> target) {
        return std::make_unique<UseTalentOnObjectAction>(_game, _services, std::move(chosenTalent), std::move(target));
    }

    std::unique_ptr<WaitAction> newWait(float seconds) {
        return std::make_unique<WaitAction>(_game, _services, seconds);
    }

private:
    Game &_game;
    Services &_services;
};

} // namespace game

} // namespace reone
