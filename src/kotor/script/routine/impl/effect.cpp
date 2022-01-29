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

/** @file
 *  Implementation of effect factory routines.
 */

#include "../declarations.h"

#include "../../../../game/effect.h"
#include "../../../../game/effect/factory.h"
#include "../../../../game/game.h"
#include "../../../../game/services.h"
#include "../../../../game/types.h"
#include "../../../../script/exception/notimpl.h"

#include "../argutil.h"
#include "../context.h"

using namespace std;

using namespace reone::game;
using namespace reone::script;

namespace reone {

namespace kotor {

namespace routine {

Variable effectAssuredHit(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newAssuredHit();
    return Variable::ofEffect(move(effect));
}

Variable effectHeal(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newHeal();
    return Variable::ofEffect(move(effect));
}

Variable effectDamage(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newDamage(0, DamageType::Universal, nullptr);
    return Variable::ofEffect(move(effect));
}

Variable effectAbilityIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newAbilityIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageResistance(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newDamageResistance();
    return Variable::ofEffect(move(effect));
}

Variable effectResurrection(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newResurrection();
    return Variable::ofEffect(move(effect));
}

Variable effectACIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newACIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectSavingThrowIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newSavingThrowIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectAttackIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newAttackIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newDamageIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectEntangle(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newEntangle();
    return Variable::ofEffect(move(effect));
}

Variable effectDeath(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newDeath();
    return Variable::ofEffect(std::move(effect));
}

Variable effectParalyze(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newParalyze();
    return Variable::ofEffect(move(effect));
}

Variable effectSleep(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newSleep();
    return Variable::ofEffect(move(effect));
}

Variable effectTemporaryForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newTemporaryForcePoints();
    return Variable::ofEffect(move(effect));
}

Variable effectChoke(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newChoke();
    return Variable::ofEffect(move(effect));
}

Variable effectStunned(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newStunned();
    return Variable::ofEffect(move(effect));
}

Variable effectRegenerate(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newRegenerate();
    return Variable::ofEffect(move(effect));
}

Variable effectMovementSpeedIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newMovementSpeedIncrease();
    return Variable::ofEffect(move(effect));
}

Variable getEffectType(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable effectVisualEffect(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newVisual();
    return Variable::ofEffect(move(effect));
}

Variable effectLinkEffects(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newLinkEffects();
    return Variable::ofEffect(move(effect));
}

Variable effectBeam(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newBeam();
    return Variable::ofEffect(move(effect));
}

Variable effectForceResistanceIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newForceResistanceIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectBodyFuel(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newBodyFuel();
    return Variable::ofEffect(move(effect));
}

Variable effectPoison(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newPoison();
    return Variable::ofEffect(move(effect));
}

Variable effectForcePushTargeted(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newForcePushTargeted();
    return Variable::ofEffect(move(effect));
}

Variable effectImmunity(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newImmunity();
    return Variable::ofEffect(move(effect));
}

Variable getEffectSpellId(const vector<Variable> &args, const RoutineContext &ctx) {
    throw NotImplementedException();
}

Variable effectDamageImmunityIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newDamageImmunityIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectTemporaryHitpoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newTemporaryHitPoints();
    return Variable::ofEffect(move(effect));
}

Variable effectSkillIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newSkillIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newDamageForcePoints();
    return Variable::ofEffect(move(effect));
}

Variable effectHealForcePoints(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newHealForcePoints();
    return Variable::ofEffect(move(effect));
}

Variable effectDroidStun(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newDroidStun();
    return Variable::ofEffect(move(effect));
}

Variable effectForcePushed(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newForcePushed();
    return Variable::ofEffect(move(effect));
}

Variable effectForceResisted(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newForceResisted();
    return Variable::ofEffect(move(effect));
}

Variable effectForceFizzle(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newForceFizzle();
    return Variable::ofEffect(move(effect));
}

Variable effectAbilityDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newAbilityDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectAttackDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newAttackDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectDamageDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newDamageDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectACDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newACDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectMovementSpeedDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newMovementSpeedDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectSavingThrowDecrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newSavingThrowDecrease();
    return Variable::ofEffect(move(effect));
}

Variable effectInvisibility(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newInvisibility();
    return Variable::ofEffect(move(effect));
}

Variable effectForceShield(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newForceShield();
    return Variable::ofEffect(move(effect));
}

Variable effectDisguise(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newDisguise();
    return Variable::ofEffect(move(effect));
}

Variable effectTrueSeeing(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newTrueSeeing();
    return Variable::ofEffect(move(effect));
}

Variable effectBlasterDeflectionIncrease(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newBlasterDeflectionIncrease();
    return Variable::ofEffect(move(effect));
}

Variable effectHorrified(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newHorrified();
    return Variable::ofEffect(move(effect));
}

Variable effectModifyAttacks(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newModifyAttacks();
    return Variable::ofEffect(move(effect));
}

Variable effectPsychicStatic(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newPsychicStatic();
    return Variable::ofEffect(move(effect));
}

Variable effectLightsaberThrow(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newLightsaberThrow();
    return Variable::ofEffect(move(effect));
}

Variable effectWhirlWind(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newWhirlWind();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneHorrified(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newCutsceneHorrified();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneParalyze(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newCutsceneParalyze();
    return Variable::ofEffect(move(effect));
}

Variable effectCutSceneStunned(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newCutsceneStunned();
    return Variable::ofEffect(move(effect));
}

// TSL

Variable effectForceBody(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newForceBody();
    return Variable::ofEffect(move(effect));
}

Variable effectFury(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newFury();
    return Variable::ofEffect(move(effect));
}

Variable effectBlind(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newBlind();
    return Variable::ofEffect(move(effect));
}

Variable effectVPRegenModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newVPRegenModifier();
    return Variable::ofEffect(move(effect));
}

Variable effectCrush(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newCrush();
    return Variable::ofEffect(move(effect));
}

Variable effectForceSight(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newForceSight();
    return Variable::ofEffect(move(effect));
}

Variable effectMindTrick(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newMindTrick();
    return Variable::ofEffect(move(effect));
}

Variable effectFactionModifier(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newFactionModifier();
    return Variable::ofEffect(move(effect));
}

Variable effectDroidScramble(const vector<Variable> &args, const RoutineContext &ctx) {
    auto effect = ctx.game.effectFactory().newDroidScramble();
    return Variable::ofEffect(move(effect));
}

// END TSL

} // namespace routine

} // namespace kotor

} // namespace reone
