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

#include "abilitydecrease.h"
#include "abilityincrease.h"
#include "acdecrease.h"
#include "acincrease.h"
#include "areaofeffect.h"
#include "assureddeflection.h"
#include "assuredhit.h"
#include "attackdecrease.h"
#include "attackincrease.h"
#include "beam.h"
#include "blasterdeflectiondecrease.h"
#include "blasterdeflectionincrease.h"
#include "blind.h"
#include "bodyfuel.h"
#include "choke.h"
#include "concealment.h"
#include "confused.h"
#include "crush.h"
#include "cutscenehorrified.h"
#include "cutsceneparalyze.h"
#include "cutscenestunned.h"
#include "damage.h"
#include "damagedecrease.h"
#include "damageforcepoints.h"
#include "damageimmunitydecrease.h"
#include "damageimmunityincrease.h"
#include "damageincrease.h"
#include "damagereduction.h"
#include "damageresistance.h"
#include "damageshield.h"
#include "death.h"
#include "disguise.h"
#include "dispelmagicall.h"
#include "dispelmagicbest.h"
#include "droidconfused.h"
#include "droidscramble.h"
#include "droidstun.h"
#include "entangle.h"
#include "factionmodifier.h"
#include "forcebody.h"
#include "forcedrain.h"
#include "forcefizzle.h"
#include "forcejump.h"
#include "forcepushed.h"
#include "forcepushtargeted.h"
#include "forceresistancedecrease.h"
#include "forceresistanceincrease.h"
#include "forceresisted.h"
#include "forceshield.h"
#include "forcesight.h"
#include "fpregenmodifier.h"
#include "frightened.h"
#include "fury.h"
#include "haste.h"
#include "heal.h"
#include "healforcepoints.h"
#include "hitpointschangewhendying.h"
#include "horrified.h"
#include "immunity.h"
#include "invisibility.h"
#include "knockdown.h"
#include "lightsaberthrow.h"
#include "linkeffects.h"
#include "mindtrick.h"
#include "misschance.h"
#include "modifyattacks.h"
#include "movementspeeddecrease.h"
#include "movementspeedincrease.h"
#include "paralyze.h"
#include "poison.h"
#include "psychicstatic.h"
#include "regenerate.h"
#include "resurrection.h"
#include "savingthrowdecrease.h"
#include "savingthrowincrease.h"
#include "seeinvisible.h"
#include "skilldecrease.h"
#include "skillincrease.h"
#include "sleep.h"
#include "spellimmunity.h"
#include "spelllevelabsorption.h"
#include "stunned.h"
#include "temporaryforcepoints.h"
#include "temporaryhitpoints.h"
#include "timestop.h"
#include "trueseeing.h"
#include "visual.h"
#include "vpregenmodifier.h"
#include "whirlwind.h"

namespace reone {

namespace game {

class EffectFactory {
public:
    std::unique_ptr<AbilityDecreaseEffect> newAbilityDecrease() {
        return std::make_unique<AbilityDecreaseEffect>();
    }

    std::unique_ptr<AbilityIncreaseEffect> newAbilityIncrease() {
        return std::make_unique<AbilityIncreaseEffect>();
    }

    std::unique_ptr<ACDecreaseEffect> newACDecrease() {
        return std::make_unique<ACDecreaseEffect>();
    }

    std::unique_ptr<ACIncreaseEffect> newACIncrease() {
        return std::make_unique<ACIncreaseEffect>();
    }

    std::unique_ptr<AreaOfEffectEffect> newAreaOfEffect() {
        return std::make_unique<AreaOfEffectEffect>();
    }

    std::unique_ptr<AssuredDeflectionEffect> newAssuredDeflection() {
        return std::make_unique<AssuredDeflectionEffect>();
    }

    std::unique_ptr<AssuredHitEffect> newAssuredHit() {
        return std::make_unique<AssuredHitEffect>();
    }

    std::unique_ptr<AttackDecreaseEffect> newAttackDecrease() {
        return std::make_unique<AttackDecreaseEffect>();
    }

    std::unique_ptr<AttackIncreaseEffect> newAttackIncrease() {
        return std::make_unique<AttackIncreaseEffect>();
    }

    std::unique_ptr<BeamEffect> newBeam() {
        return std::make_unique<BeamEffect>();
    }

    std::unique_ptr<BlasterDeflectionDecreaseEffect> newBlasterDeflectionDecrease() {
        return std::make_unique<BlasterDeflectionDecreaseEffect>();
    }

    std::unique_ptr<BlasterDeflectionIncreaseEffect> newBlasterDeflectionIncrease() {
        return std::make_unique<BlasterDeflectionIncreaseEffect>();
    }

    std::unique_ptr<BlindEffect> newBlind() {
        return std::make_unique<BlindEffect>();
    }

    std::unique_ptr<BodyFuelEffect> newBodyFuel() {
        return std::make_unique<BodyFuelEffect>();
    }

    std::unique_ptr<ChokeEffect> newChoke() {
        return std::make_unique<ChokeEffect>();
    }

    std::unique_ptr<ConcealmentEffect> newConcealment() {
        return std::make_unique<ConcealmentEffect>();
    }

    std::unique_ptr<ConfusedEffect> newConfused() {
        return std::make_unique<ConfusedEffect>();
    }

    std::unique_ptr<CrushEffect> newCrush() {
        return std::make_unique<CrushEffect>();
    }

    std::unique_ptr<CutsceneHorrifiedEffect> newCutsceneHorrified() {
        return std::make_unique<CutsceneHorrifiedEffect>();
    }

    std::unique_ptr<CutsceneParalyzeEffect> newCutsceneParalyze() {
        return std::make_unique<CutsceneParalyzeEffect>();
    }

    std::unique_ptr<CutsceneStunnedEffect> newCutsceneStunned() {
        return std::make_unique<CutsceneStunnedEffect>();
    }

    std::unique_ptr<DamageEffect> newDamage(int amount, DamageType type, std::shared_ptr<Creature> damager) {
        return std::make_unique<DamageEffect>(amount, type, std::move(damager));
    }

    std::unique_ptr<DamageDecreaseEffect> newDamageDecrease() {
        return std::make_unique<DamageDecreaseEffect>();
    }

    std::unique_ptr<DamageForcePointsEffect> newDamageForcePoints() {
        return std::make_unique<DamageForcePointsEffect>();
    }

    std::unique_ptr<DamageImmunityDecreaseEffect> newDamageImmunityDecrease() {
        return std::make_unique<DamageImmunityDecreaseEffect>();
    }

    std::unique_ptr<DamageImmunityIncreaseEffect> newDamageImmunityIncrease() {
        return std::make_unique<DamageImmunityIncreaseEffect>();
    }

    std::unique_ptr<DamageIncreaseEffect> newDamageIncrease() {
        return std::make_unique<DamageIncreaseEffect>();
    }

    std::unique_ptr<DamageReductionEffect> newDamageReduction() {
        return std::make_unique<DamageReductionEffect>();
    }

    std::unique_ptr<DamageResistanceEffect> newDamageResistance() {
        return std::make_unique<DamageResistanceEffect>();
    }

    std::unique_ptr<DamageShieldEffect> newDamageShield() {
        return std::make_unique<DamageShieldEffect>();
    }

    std::unique_ptr<DeathEffect> newDeath() {
        return std::make_unique<DeathEffect>();
    }

    std::unique_ptr<DisguiseEffect> newDisguise() {
        return std::make_unique<DisguiseEffect>();
    }

    std::unique_ptr<DispelMagicAllEffect> newDispelMagicAll() {
        return std::make_unique<DispelMagicAllEffect>();
    }

    std::unique_ptr<DispelMagicBestEffect> newDispelMagicBest() {
        return std::make_unique<DispelMagicBestEffect>();
    }

    std::unique_ptr<DroidConfusedEffect> newDroidConfused() {
        return std::make_unique<DroidConfusedEffect>();
    }

    std::unique_ptr<DroidScrambleEffect> newDroidScramble() {
        return std::make_unique<DroidScrambleEffect>();
    }

    std::unique_ptr<DroidStunEffect> newDroidStun() {
        return std::make_unique<DroidStunEffect>();
    }

    std::unique_ptr<EntangleEffect> newEntangle() {
        return std::make_unique<EntangleEffect>();
    }

    std::unique_ptr<FactionModifierEffect> newFactionModifier() {
        return std::make_unique<FactionModifierEffect>();
    }

    std::unique_ptr<ForceBodyEffect> newForceBody() {
        return std::make_unique<ForceBodyEffect>();
    }

    std::unique_ptr<ForceDrainEffect> newForceDrain() {
        return std::make_unique<ForceDrainEffect>();
    }

    std::unique_ptr<ForceFizzleEffect> newForceFizzle() {
        return std::make_unique<ForceFizzleEffect>();
    }

    std::unique_ptr<ForceJumpEffect> newForceJump() {
        return std::make_unique<ForceJumpEffect>();
    }

    std::unique_ptr<ForcePushedEffect> newForcePushed() {
        return std::make_unique<ForcePushedEffect>();
    }

    std::unique_ptr<ForcePushTargetedEffect> newForcePushTargeted() {
        return std::make_unique<ForcePushTargetedEffect>();
    }

    std::unique_ptr<ForceResistanceDecreaseEffect> newForceResistanceDecrease() {
        return std::make_unique<ForceResistanceDecreaseEffect>();
    }

    std::unique_ptr<ForceResistanceIncreaseEffect> newForceResistanceIncrease() {
        return std::make_unique<ForceResistanceIncreaseEffect>();
    }

    std::unique_ptr<ForceResistedEffect> newForceResisted() {
        return std::make_unique<ForceResistedEffect>();
    }

    std::unique_ptr<ForceShieldEffect> newForceShield() {
        return std::make_unique<ForceShieldEffect>();
    }

    std::unique_ptr<ForceSightEffect> newForceSight() {
        return std::make_unique<ForceSightEffect>();
    }

    std::unique_ptr<FPRegenModifierEffect> newFPRegenModifier() {
        return std::make_unique<FPRegenModifierEffect>();
    }

    std::unique_ptr<FrightenedEffect> newFrightened() {
        return std::make_unique<FrightenedEffect>();
    }

    std::unique_ptr<FuryEffect> newFury() {
        return std::make_unique<FuryEffect>();
    }

    std::unique_ptr<HasteEffect> newHaste() {
        return std::make_unique<HasteEffect>();
    }

    std::unique_ptr<HealEffect> newHeal() {
        return std::make_unique<HealEffect>();
    }

    std::unique_ptr<HealForcePointsEffect> newHealForcePoints() {
        return std::make_unique<HealForcePointsEffect>();
    }

    std::unique_ptr<HitPointsChangeWhenDyingEffect> newHitPointsChangeWhenDying() {
        return std::make_unique<HitPointsChangeWhenDyingEffect>();
    }

    std::unique_ptr<HorrifiedEffect> newHorrified() {
        return std::make_unique<HorrifiedEffect>();
    }

    std::unique_ptr<ImmunityEffect> newImmunity() {
        return std::make_unique<ImmunityEffect>();
    }

    std::unique_ptr<InvisibilityEffect> newInvisibility() {
        return std::make_unique<InvisibilityEffect>();
    }

    std::unique_ptr<KnockdownEffect> newKnockdown() {
        return std::make_unique<KnockdownEffect>();
    }

    std::unique_ptr<LightsaberThrowEffect> newLightsaberThrow() {
        return std::make_unique<LightsaberThrowEffect>();
    }

    std::unique_ptr<LinkEffectsEffect> newLinkEffects() {
        return std::make_unique<LinkEffectsEffect>();
    }

    std::unique_ptr<MindTrickEffect> newMindTrick() {
        return std::make_unique<MindTrickEffect>();
    }

    std::unique_ptr<MissChanceEffect> newMissChance() {
        return std::make_unique<MissChanceEffect>();
    }

    std::unique_ptr<ModifyAttacksEffect> newModifyAttacks() {
        return std::make_unique<ModifyAttacksEffect>();
    }

    std::unique_ptr<MovementSpeedDecreaseEffect> newMovementSpeedDecrease() {
        return std::make_unique<MovementSpeedDecreaseEffect>();
    }

    std::unique_ptr<MovementSpeedIncreaseEffect> newMovementSpeedIncrease() {
        return std::make_unique<MovementSpeedIncreaseEffect>();
    }

    std::unique_ptr<ParalyzeEffect> newParalyze() {
        return std::make_unique<ParalyzeEffect>();
    }

    std::unique_ptr<PoisonEffect> newPoison() {
        return std::make_unique<PoisonEffect>();
    }

    std::unique_ptr<PsychicStaticEffect> newPsychicStatic() {
        return std::make_unique<PsychicStaticEffect>();
    }

    std::unique_ptr<RegenerateEffect> newRegenerate() {
        return std::make_unique<RegenerateEffect>();
    }

    std::unique_ptr<ResurrectionEffect> newResurrection() {
        return std::make_unique<ResurrectionEffect>();
    }

    std::unique_ptr<SavingThrowDecreaseEffect> newSavingThrowDecrease() {
        return std::make_unique<SavingThrowDecreaseEffect>();
    }

    std::unique_ptr<SavingThrowIncreaseEffect> newSavingThrowIncrease() {
        return std::make_unique<SavingThrowIncreaseEffect>();
    }

    std::unique_ptr<SeeInvisibleEffect> newSeeInvisible() {
        return std::make_unique<SeeInvisibleEffect>();
    }

    std::unique_ptr<SkillDecreaseEffect> newSkillDecrease() {
        return std::make_unique<SkillDecreaseEffect>();
    }

    std::unique_ptr<SkillIncreaseEffect> newSkillIncrease() {
        return std::make_unique<SkillIncreaseEffect>();
    }

    std::unique_ptr<SleepEffect> newSleep() {
        return std::make_unique<SleepEffect>();
    }

    std::unique_ptr<SpellImmunityEffect> newSpellImmunity() {
        return std::make_unique<SpellImmunityEffect>();
    }

    std::unique_ptr<SpellLevelAbsorptionEffect> newSpellLevelAbsorption() {
        return std::make_unique<SpellLevelAbsorptionEffect>();
    }

    std::unique_ptr<StunnedEffect> newStunned() {
        return std::make_unique<StunnedEffect>();
    }

    std::unique_ptr<TemporaryForcePointsEffect> newTemporaryForcePoints() {
        return std::make_unique<TemporaryForcePointsEffect>();
    }

    std::unique_ptr<TemporaryHitPointsEffect> newTemporaryHitPoints() {
        return std::make_unique<TemporaryHitPointsEffect>();
    }

    std::unique_ptr<TimeStopEffect> newTimeStop() {
        return std::make_unique<TimeStopEffect>();
    }

    std::unique_ptr<TrueSeeingEffect> newTrueSeeing() {
        return std::make_unique<TrueSeeingEffect>();
    }

    std::unique_ptr<VisualEffect> newVisual() {
        return std::make_unique<VisualEffect>();
    }

    std::unique_ptr<VPRegenModifierEffect> newVPRegenModifier() {
        return std::make_unique<VPRegenModifierEffect>();
    }

    std::unique_ptr<WhirlWindEffect> newWhirlWind() {
        return std::make_unique<WhirlWindEffect>();
    }
};

} // namespace game

} // namespace reone