#!/usr/bin/env python3

# Copyright (c) 2020-2021 The reone project contributors

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

"""
Analyzes script routine calls by inspecting .pcode files in extraction directory.
"""

import glob
import os
import tkinter
import tkinter.filedialog

ROUTINES = [
    "Random",
    "PrintString",
    "PrintFloat",
    "FloatToString",
    "PrintInteger",
    "PrintObject",
    "AssignCommand",
    "DelayCommand",
    "ExecuteScript",
    "ClearAllActions",
    "SetFacing",
    "SwitchPlayerCharacter",
    "SetTime",
    "SetPartyLeader",
    "SetAreaUnescapable",
    "GetAreaUnescapable",
    "GetTimeHour",
    "GetTimeMinute",
    "GetTimeSecond",
    "GetTimeMillisecond",
    "ActionRandomWalk",
    "ActionMoveToLocation",
    "ActionMoveToObject",
    "ActionMoveAwayFromObject",
    "GetArea",
    "GetEnteringObject",
    "GetExitingObject",
    "GetPosition",
    "GetFacing",
    "GetItemPossessor",
    "GetItemPossessedBy",
    "CreateItemOnObject",
    "ActionEquipItem",
    "ActionUnequipItem",
    "ActionPickUpItem",
    "ActionPutDownItem",
    "GetLastAttacker",
    "ActionAttack",
    "GetNearestCreature",
    "ActionSpeakString",
    "ActionPlayAnimation",
    "GetDistanceToObject",
    "GetIsObjectValid",
    "ActionOpenDoor",
    "ActionCloseDoor",
    "SetCameraFacing",
    "PlaySound",
    "GetSpellTargetObject",
    "ActionCastSpellAtObject",
    "GetCurrentHitPoints",
    "GetMaxHitPoints",
    "EffectAssuredHit",
    "GetLastItemEquipped",
    "GetSubScreenID",
    "CancelCombat",
    "GetCurrentForcePoints",
    "GetMaxForcePoints",
    "PauseGame",
    "SetPlayerRestrictMode",
    "GetStringLength",
    "GetStringUpperCase",
    "GetStringLowerCase",
    "GetStringRight",
    "GetStringLeft",
    "InsertString",
    "GetSubString",
    "FindSubString",
    "fabs",
    "cos",
    "sin",
    "tan",
    "acos",
    "asin",
    "atan",
    "log",
    "pow",
    "sqrt",
    "abs",
    "EffectHeal",
    "EffectDamage",
    "EffectAbilityIncrease",
    "EffectDamageResistance",
    "EffectResurrection",
    "GetPlayerRestrictMode",
    "GetCasterLevel",
    "GetFirstEffect",
    "GetNextEffect",
    "RemoveEffect",
    "GetIsEffectValid",
    "GetEffectDurationType",
    "GetEffectSubType",
    "GetEffectCreator",
    "IntToString",
    "GetFirstObjectInArea",
    "GetNextObjectInArea",
    "d2",
    "d3",
    "d4",
    "d6",
    "d8",
    "d10",
    "d12",
    "d20",
    "d100",
    "VectorMagnitude",
    "GetMetaMagicFeat",
    "GetObjectType",
    "GetRacialType",
    "FortitudeSave",
    "ReflexSave",
    "WillSave",
    "GetSpellSaveDC",
    "MagicalEffect",
    "SupernaturalEffect",
    "ExtraordinaryEffect",
    "EffectACIncrease",
    "GetAC",
    "EffectSavingThrowIncrease",
    "EffectAttackIncrease",
    "EffectDamageReduction",
    "EffectDamageIncrease",
    "RoundsToSeconds",
    "HoursToSeconds",
    "TurnsToSeconds",
    "SoundObjectSetFixedVariance",
    "GetGoodEvilValue",
    "GetPartyMemberCount",
    "GetAlignmentGoodEvil",
    "GetFirstObjectInShape",
    "GetNextObjectInShape",
    "EffectEntangle",
    "SignalEvent",
    "EventUserDefined",
    "EffectDeath",
    "EffectKnockdown",
    "ActionGiveItem",
    "ActionTakeItem",
    "VectorNormalize",
    "GetItemStackSize",
    "GetAbilityScore",
    "GetIsDead",
    "PrintVector",
    "Vector",
    "SetFacingPoint",
    "AngleToVector",
    "VectorToAngle",
    "TouchAttackMelee",
    "TouchAttackRanged",
    "EffectParalyze",
    "EffectSpellImmunity",
    "SetItemStackSize",
    "GetDistanceBetween",
    "SetReturnStrref",
    "EffectForceJump",
    "EffectSleep",
    "GetItemInSlot",
    "EffectTemporaryForcePoints",
    "EffectConfused",
    "EffectFrightened",
    "EffectChoke",
    "SetGlobalString",
    "EffectStunned",
    "SetCommandable",
    "GetCommandable",
    "EffectRegenerate",
    "EffectMovementSpeedIncrease",
    "GetHitDice",
    "ActionForceFollowObject",
    "GetTag",
    "ResistForce",
    "GetEffectType",
    "EffectAreaOfEffect",
    "GetFactionEqual",
    "ChangeFaction",
    "GetIsListening",
    "SetListening",
    "SetListenPattern",
    "TestStringAgainstPattern",
    "GetMatchedSubstring",
    "GetMatchedSubstringsCount",
    "EffectVisualEffect",
    "GetFactionWeakestMember",
    "GetFactionStrongestMember",
    "GetFactionMostDamagedMember",
    "GetFactionLeastDamagedMember",
    "GetFactionGold",
    "GetFactionAverageReputation",
    "GetFactionAverageGoodEvilAlignment",
    "SoundObjectGetFixedVariance",
    "GetFactionAverageLevel",
    "GetFactionAverageXP",
    "GetFactionMostFrequentClass",
    "GetFactionWorstAC",
    "GetFactionBestAC",
    "GetGlobalString",
    "GetListenPatternNumber",
    "ActionJumpToObject",
    "GetWaypointByTag",
    "GetTransitionTarget",
    "EffectLinkEffects",
    "GetObjectByTag",
    "AdjustAlignment",
    "ActionWait",
    "SetAreaTransitionBMP",
    "ActionStartConversation",
    "ActionPauseConversation",
    "ActionResumeConversation",
    "EffectBeam",
    "GetReputation",
    "AdjustReputation",
    "GetModuleFileName",
    "GetGoingToBeAttackedBy",
    "EffectForceResistanceIncrease",
    "GetLocation",
    "ActionJumpToLocation",
    "Location",
    "ApplyEffectAtLocation",
    "GetIsPC",
    "FeetToMeters",
    "YardsToMeters",
    "ApplyEffectToObject",
    "SpeakString",
    "GetSpellTargetLocation",
    "GetPositionFromLocation",
    "EffectBodyFuel",
    "GetFacingFromLocation",
    "GetNearestCreatureToLocation",
    "GetNearestObject",
    "GetNearestObjectToLocation",
    "GetNearestObjectByTag",
    "IntToFloat",
    "FloatToInt",
    "StringToInt",
    "StringToFloat",
    "ActionCastSpellAtLocation",
    "GetIsEnemy",
    "GetIsFriend",
    "GetIsNeutral",
    "GetPCSpeaker",
    "GetStringByStrRef",
    "ActionSpeakStringByStrRef",
    "DestroyObject",
    "GetModule",
    "CreateObject",
    "EventSpellCastAt",
    "GetLastSpellCaster",
    "GetLastSpell",
    "GetUserDefinedEventNumber",
    "GetSpellId",
    "RandomName",
    "EffectPoison",
    "GetLoadFromSaveGame",
    "EffectAssuredDeflection",
    "GetName",
    "GetLastSpeaker",
    "BeginConversation",
    "GetLastPerceived",
    "GetLastPerceptionHeard",
    "GetLastPerceptionInaudible",
    "GetLastPerceptionSeen",
    "GetLastClosedBy",
    "GetLastPerceptionVanished",
    "GetFirstInPersistentObject",
    "GetNextInPersistentObject",
    "GetAreaOfEffectCreator",
    "ShowLevelUpGUI",
    "SetItemNonEquippable",
    "GetButtonMashCheck",
    "SetButtonMashCheck",
    "EffectForcePushTargeted",
    "EffectHaste",
    "GiveItem",
    "ObjectToString",
    "EffectImmunity",
    "GetIsImmune",
    "EffectDamageImmunityIncrease",
    "GetEncounterActive",
    "SetEncounterActive",
    "GetEncounterSpawnsMax",
    "SetEncounterSpawnsMax",
    "GetEncounterSpawnsCurrent",
    "SetEncounterSpawnsCurrent",
    "GetModuleItemAcquired",
    "GetModuleItemAcquiredFrom",
    "SetCustomToken",
    "GetHasFeat",
    "GetHasSkill",
    "ActionUseFeat",
    "ActionUseSkill",
    "GetObjectSeen",
    "GetObjectHeard",
    "GetLastPlayerDied",
    "GetModuleItemLost",
    "GetModuleItemLostBy",
    "ActionDoCommand",
    "EventConversation",
    "SetEncounterDifficulty",
    "GetEncounterDifficulty",
    "GetDistanceBetweenLocations",
    "GetReflexAdjustedDamage",
    "PlayAnimation",
    "TalentSpell",
    "TalentFeat",
    "TalentSkill",
    "GetHasSpellEffect",
    "GetEffectSpellId",
    "GetCreatureHasTalent",
    "GetCreatureTalentRandom",
    "GetCreatureTalentBest",
    "ActionUseTalentOnObject",
    "ActionUseTalentAtLocation",
    "GetGoldPieceValue",
    "GetIsPlayableRacialType",
    "JumpToLocation",
    "EffectTemporaryHitpoints",
    "GetSkillRank",
    "GetAttackTarget",
    "GetLastAttackType",
    "GetLastAttackMode",
    "GetDistanceBetween2D",
    "GetIsInCombat",
    "GetLastAssociateCommand",
    "GiveGoldToCreature",
    "SetIsDestroyable",
    "SetLocked",
    "GetLocked",
    "GetClickingObject",
    "SetAssociateListenPatterns",
    "GetLastWeaponUsed",
    "ActionInteractObject",
    "GetLastUsedBy",
    "GetAbilityModifier",
    "GetIdentified",
    "SetIdentified",
    "GetDistanceBetweenLocations2D",
    "GetDistanceToObject2D",
    "GetBlockingDoor",
    "GetIsDoorActionPossible",
    "DoDoorAction",
    "GetFirstItemInInventory",
    "GetNextItemInInventory",
    "GetClassByPosition",
    "GetLevelByPosition",
    "GetLevelByClass",
    "GetDamageDealtByType",
    "GetTotalDamageDealt",
    "GetLastDamager",
    "GetLastDisarmed",
    "GetLastDisturbed",
    "GetLastLocked",
    "GetLastUnlocked",
    "EffectSkillIncrease",
    "GetInventoryDisturbType",
    "GetInventoryDisturbItem",
    "ShowUpgradeScreen",
    "VersusAlignmentEffect",
    "VersusRacialTypeEffect",
    "VersusTrapEffect",
    "GetGender",
    "GetIsTalentValid",
    "ActionMoveAwayFromLocation",
    "GetAttemptedAttackTarget",
    "GetTypeFromTalent",
    "GetIdFromTalent",
    "PlayPazaak",
    "GetLastPazaakResult",
    "DisplayFeedBackText",
    "AddJournalQuestEntry",
    "RemoveJournalQuestEntry",
    "GetJournalEntry",
    "PlayRumblePattern",
    "StopRumblePattern",
    "EffectDamageForcePoints",
    "EffectHealForcePoints",
    "SendMessageToPC",
    "GetAttemptedSpellTarget",
    "GetLastOpenedBy",
    "GetHasSpell",
    "OpenStore",
    "ActionSurrenderToEnemies",
    "GetFirstFactionMember",
    "GetNextFactionMember",
    "ActionForceMoveToLocation",
    "ActionForceMoveToObject",
    "GetJournalQuestExperience",
    "JumpToObject",
    "SetMapPinEnabled",
    "EffectHitPointChangeWhenDying",
    "PopUpGUIPanel",
    "AddMultiClass",
    "GetIsLinkImmune",
    "EffectDroidStun",
    "EffectForcePushed",
    "GiveXPToCreature",
    "SetXP",
    "GetXP",
    "IntToHexString",
    "GetBaseItemType",
    "GetItemHasItemProperty",
    "ActionEquipMostDamagingMelee",
    "ActionEquipMostDamagingRanged",
    "GetItemACValue",
    "EffectForceResisted",
    "ExploreAreaForPlayer",
    "ActionEquipMostEffectiveArmor",
    "GetIsDay",
    "GetIsNight",
    "GetIsDawn",
    "GetIsDusk",
    "GetIsEncounterCreature",
    "GetLastPlayerDying",
    "GetStartingLocation",
    "ChangeToStandardFaction",
    "SoundObjectPlay",
    "SoundObjectStop",
    "SoundObjectSetVolume",
    "SoundObjectSetPosition",
    "SpeakOneLinerConversation",
    "GetGold",
    "GetLastRespawnButtonPresser",
    "EffectForceFizzle",
    "SetLightsaberPowered",
    "GetIsWeaponEffective",
    "GetLastSpellHarmful",
    "EventActivateItem",
    "MusicBackgroundPlay",
    "MusicBackgroundStop",
    "MusicBackgroundSetDelay",
    "MusicBackgroundChangeDay",
    "MusicBackgroundChangeNight",
    "MusicBattlePlay",
    "MusicBattleStop",
    "MusicBattleChange",
    "AmbientSoundPlay",
    "AmbientSoundStop",
    "AmbientSoundChangeDay",
    "AmbientSoundChangeNight",
    "GetLastKiller",
    "GetSpellCastItem",
    "GetItemActivated",
    "GetItemActivator",
    "GetItemActivatedTargetLocation",
    "GetItemActivatedTarget",
    "GetIsOpen",
    "TakeGoldFromCreature",
    "GetIsInConversation",
    "EffectAbilityDecrease",
    "EffectAttackDecrease",
    "EffectDamageDecrease",
    "EffectDamageImmunityDecrease",
    "EffectACDecrease",
    "EffectMovementSpeedDecrease",
    "EffectSavingThrowDecrease",
    "EffectSkillDecrease",
    "EffectForceResistanceDecrease",
    "GetPlotFlag",
    "SetPlotFlag",
    "EffectInvisibility",
    "EffectConcealment",
    "EffectForceShield",
    "EffectDispelMagicAll",
    "SetDialogPlaceableCamera",
    "GetSoloMode",
    "EffectDisguise",
    "GetMaxStealthXP",
    "EffectTrueSeeing",
    "EffectSeeInvisible",
    "EffectTimeStop",
    "SetMaxStealthXP",
    "EffectBlasterDeflectionIncrease",
    "EffectBlasterDeflectionDecrease",
    "EffectHorrified",
    "EffectSpellLevelAbsorption",
    "EffectDispelMagicBest",
    "GetCurrentStealthXP",
    "GetNumStackedItems",
    "SurrenderToEnemies",
    "EffectMissChance",
    "SetCurrentStealthXP",
    "GetCreatureSize",
    "AwardStealthXP",
    "GetStealthXPEnabled",
    "SetStealthXPEnabled",
    "ActionUnlockObject",
    "ActionLockObject",
    "EffectModifyAttacks",
    "GetLastTrapDetected",
    "EffectDamageShield",
    "GetNearestTrapToObject",
    "GetAttemptedMovementTarget",
    "GetBlockingCreature",
    "GetFortitudeSavingThrow",
    "GetWillSavingThrow",
    "GetReflexSavingThrow",
    "GetChallengeRating",
    "GetFoundEnemyCreature",
    "GetMovementRate",
    "GetSubRace",
    "GetStealthXPDecrement",
    "SetStealthXPDecrement",
    "DuplicateHeadAppearance",
    "ActionCastFakeSpellAtObject",
    "ActionCastFakeSpellAtLocation",
    "CutsceneAttack",
    "SetCameraMode",
    "SetLockOrientationInDialog",
    "SetLockHeadFollowInDialog",
    "CutsceneMove",
    "EnableVideoEffect",
    "StartNewModule",
    "DisableVideoEffect",
    "GetWeaponRanged",
    "DoSinglePlayerAutoSave",
    "GetGameDifficulty",
    "GetUserActionsPending",
    "RevealMap",
    "SetTutorialWindowsEnabled",
    "ShowTutorialWindow",
    "StartCreditSequence",
    "IsCreditSequenceInProgress",

    "SWMG_SetLateralAccelerationPerSecond",
    "SWMG_GetLateralAccelerationPerSecond",

    "GetCurrentAction",
    "GetDifficultyModifier",
    "GetAppearanceType",
    "FloatingTextStrRefOnCreature",
    "FloatingTextStringOnCreature",
    "GetTrapDisarmable",
    "GetTrapDetectable",
    "GetTrapDetectedBy",
    "GetTrapFlagged",
    "GetTrapBaseType",
    "GetTrapOneShot",
    "GetTrapCreator",
    "GetTrapKeyTag",
    "GetTrapDisarmDC",
    "GetTrapDetectDC",
    "GetLockKeyRequired",
    "GetLockKeyTag",
    "GetLockLockable",
    "GetLockUnlockDC",
    "GetLockLockDC",
    "GetPCLevellingUp",
    "GetHasFeatEffect",
    "SetPlaceableIllumination",
    "GetPlaceableIllumination",
    "GetIsPlaceableObjectActionPossible",
    "DoPlaceableObjectAction",
    "GetFirstPC",
    "GetNextPC",
    "SetTrapDetectedBy",
    "GetIsTrapped",
    "SetEffectIcon",
    "FaceObjectAwayFromObject",
    "PopUpDeathGUIPanel",
    "SetTrapDisabled",
    "GetLastHostileActor",
    "ExportAllCharacters",
    "MusicBackgroundGetDayTrack",
    "MusicBackgroundGetNightTrack",
    "WriteTimestampedLogEntry",
    "GetModuleName",
    "GetFactionLeader",

    "SWMG_SetSpeedBlurEffect",

    "EndGame",
    "GetRunScriptVar",
    "GetCreatureMovmentType",
    "AmbientSoundSetDayVolume",
    "AmbientSoundSetNightVolume",
    "MusicBackgroundGetBattleTrack",
    "GetHasInventory",
    "GetStrRefSoundDuration",
    "AddToParty",
    "RemoveFromParty",
    "AddPartyMember",
    "RemovePartyMember",
    "IsObjectPartyMember",
    "GetPartyMemberByIndex",
    "GetGlobalBoolean",
    "SetGlobalBoolean",
    "GetGlobalNumber",
    "SetGlobalNumber",
    "AurPostString",

    "SWMG_GetLastEvent",
    "SWMG_GetLastEventModelName",
    "SWMG_GetObjectByName",
    "SWMG_PlayAnimation",
    "SWMG_GetLastBulletHitDamage",
    "SWMG_GetLastBulletHitTarget",
    "SWMG_GetLastBulletHitShooter",
    "SWMG_AdjustFollowerHitPoints",
    "SWMG_OnBulletHit",
    "SWMG_OnObstacleHit",
    "SWMG_GetLastFollowerHit",
    "SWMG_GetLastObstacleHit",
    "SWMG_GetLastBulletFiredDamage",
    "SWMG_GetLastBulletFiredTarget",
    "SWMG_GetObjectName",
    "SWMG_OnDeath",
    "SWMG_IsFollower",
    "SWMG_IsPlayer",
    "SWMG_IsEnemy",
    "SWMG_IsTrigger",
    "SWMG_IsObstacle",
    "SWMG_SetFollowerHitPoints",
    "SWMG_OnDamage",
    "SWMG_GetLastHPChange",
    "SWMG_RemoveAnimation",
    "SWMG_GetCameraNearClip",
    "SWMG_GetCameraFarClip",
    "SWMG_SetCameraClip",
    "SWMG_GetPlayer",
    "SWMG_GetEnemyCount",
    "SWMG_GetEnemy",
    "SWMG_GetObstacleCount",
    "SWMG_GetObstacle",
    "SWMG_GetHitPoints",
    "SWMG_GetMaxHitPoints",
    "SWMG_SetMaxHitPoints",
    "SWMG_GetSphereRadius",
    "SWMG_SetSphereRadius",
    "SWMG_GetNumLoops",
    "SWMG_SetNumLoops",
    "SWMG_GetPosition",
    "SWMG_GetGunBankCount",
    "SWMG_GetGunBankBulletModel",
    "SWMG_GetGunBankGunModel",
    "SWMG_GetGunBankDamage",
    "SWMG_GetGunBankTimeBetweenShots",
    "SWMG_GetGunBankLifespan",
    "SWMG_GetGunBankSpeed",
    "SWMG_GetGunBankTarget",
    "SWMG_SetGunBankBulletModel",
    "SWMG_SetGunBankGunModel",
    "SWMG_SetGunBankDamage",
    "SWMG_SetGunBankTimeBetweenShots",
    "SWMG_SetGunBankLifespan",
    "SWMG_SetGunBankSpeed",
    "SWMG_SetGunBankTarget",
    "SWMG_GetLastBulletHitPart",
    "SWMG_IsGunBankTargetting",
    "SWMG_GetPlayerOffset",
    "SWMG_GetPlayerInvincibility",
    "SWMG_GetPlayerSpeed",
    "SWMG_GetPlayerMinSpeed",
    "SWMG_GetPlayerAccelerationPerSecond",
    "SWMG_GetPlayerTunnelPos",
    "SWMG_SetPlayerOffset",
    "SWMG_SetPlayerInvincibility",
    "SWMG_SetPlayerSpeed",
    "SWMG_SetPlayerMinSpeed",
    "SWMG_SetPlayerAccelerationPerSecond",
    "SWMG_SetPlayerTunnelPos",
    "SWMG_GetPlayerTunnelNeg",
    "SWMG_SetPlayerTunnelNeg",
    "SWMG_GetPlayerOrigin",
    "SWMG_SetPlayerOrigin",
    "SWMG_GetGunBankHorizontalSpread",
    "SWMG_GetGunBankVerticalSpread",
    "SWMG_GetGunBankSensingRadius",
    "SWMG_GetGunBankInaccuracy",
    "SWMG_SetGunBankHorizontalSpread",
    "SWMG_SetGunBankVerticalSpread",
    "SWMG_SetGunBankSensingRadius",
    "SWMG_SetGunBankInaccuracy",
    "SWMG_GetIsInvulnerable",
    "SWMG_StartInvulnerability",
    "SWMG_GetPlayerMaxSpeed",
    "SWMG_SetPlayerMaxSpeed",

    "AddJournalWorldEntry",
    "AddJournalWorldEntryStrref",
    "BarkString",
    "DeleteJournalWorldAllEntries",
    "DeleteJournalWorldEntry",
    "DeleteJournalWorldEntryStrref",
    "EffectForceDrain",
    "EffectPsychicStatic",
    "PlayVisualAreaEffect",
    "SetJournalQuestEntryPicture",
    "GetLocalBoolean",
    "SetLocalBoolean",
    "GetLocalNumber",
    "SetLocalNumber",

    "SWMG_GetSoundFrequency",
    "SWMG_SetSoundFrequency",
    "SWMG_GetSoundFrequencyIsRandom",
    "SWMG_SetSoundFrequencyIsRandom",
    "SWMG_GetSoundVolume",
    "SWMG_SetSoundVolume",

    "SoundObjectGetPitchVariance",
    "SoundObjectSetPitchVariance",
    "SoundObjectGetVolume",
    "GetGlobalLocation",
    "SetGlobalLocation",
    "AddAvailableNPCByObject",
    "RemoveAvailableNPC",
    "IsAvailableCreature",
    "AddAvailableNPCByTemplate",
    "SpawnAvailableNPC",
    "IsNPCPartyMember",
    "ActionBarkString",
    "GetIsConversationActive",
    "EffectLightsaberThrow",
    "EffectWhirlWind",
    "GetPartyAIStyle",
    "GetNPCAIStyle",
    "SetPartyAIStyle",
    "SetNPCAIStyle",
    "SetNPCSelectability",
    "GetNPCSelectability",
    "ClearAllEffects",
    "GetLastConversation",
    "ShowPartySelectionGUI",
    "GetStandardFaction",
    "GivePlotXP",
    "GetMinOneHP",
    "SetMinOneHP",

    "SWMG_GetPlayerTunnelInfinite",
    "SWMG_SetPlayerTunnelInfinite",

    "SetGlobalFadeIn",
    "SetGlobalFadeOut",
    "GetLastHostileTarget",
    "GetLastAttackAction",
    "GetLastForcePowerUsed",
    "GetLastCombatFeatUsed",
    "GetLastAttackResult",
    "GetWasForcePowerSuccessful",
    "GetFirstAttacker",
    "GetNextAttacker",
    "SetFormation",
    "ActionFollowLeader",
    "SetForcePowerUnsuccessful",
    "GetIsDebilitated",
    "PlayMovie",
    "SaveNPCState",
    "GetCategoryFromTalent",
    "SurrenderByFaction",
    "ChangeFactionByFaction",
    "PlayRoomAnimation",
    "ShowGalaxyMap",
    "SetPlanetSelectable",
    "GetPlanetSelectable",
    "SetPlanetAvailable",
    "GetPlanetAvailable",
    "GetSelectedPlanet",
    "SoundObjectFadeAndStop",
    "SetAreaFogColor",
    "ChangeItemCost",
    "GetIsLiveContentAvailable",
    "ResetDialogState",
    "SetGoodEvilValue",
    "GetIsPoisoned",
    "GetSpellTarget",
    "SetSoloMode",
    "EffectCutSceneHorrified",
    "EffectCutSceneParalyze",
    "EffectCutSceneStunned",
    "CancelPostDialogCharacterSwitch",
    "SetMaxHitPoints",
    "NoClicksFor",
    "HoldWorldFadeInForDialog",
    "ShipBuild",
    "SurrenderRetainBuffs",
    "SuppressStatusSummaryEntry",
    "GetCheatCode",
    "SetMusicVolume",
    "CreateItemOnFloor",
    "SetAvailableNPCId",
    "GetScriptParameter",
    "SetFadeUntilScript",
    "EffectForceBody",
    "GetItemComponent",
    "GetItemComponentPieceValue",
    "ShowChemicalUpgradeScreen",
    "GetChemicals",
    "GetChemicalPieceValue",
    "GetSpellForcePointCost",
    "EffectFury",
    "EffectBlind",
    "EffectFPRegenModifier",
    "EffectVPRegenModifier",
    "EffectCrush",

    "SWMG_GetSwoopUpgrade",

    "GetFeatAcquired",
    "GetSpellAcquired",
    "ShowSwoopUpgradeScreen",
    "GrantFeat",
    "GrantSpell",
    "SpawnMine",

    "SWMG_GetTrackPosition",
    "SWMG_SetFollowerPosition",

    "SetFakeCombatState",

    "SWMG_DestroyMiniGameObject",

    "GetOwnerDemolitionsSkill",
    "SetOrientOnClick",
    "GetInfluence",
    "SetInfluence",
    "ModifyInfluence",
    "GetRacialSubType",
    "IncrementGlobalNumber",
    "DecrementGlobalNumber",
    "SetBonusForcePoints",
    "AddBonusForcePoints",
    "GetBonusForcePoints",

    "SWMG_SetJumpSpeed",

    "IsMoviePlaying",
    "QueueMovie",
    "PlayMovieQueue",
    "YavinHackDoorClose",
    "EffectDroidConfused",
    "IsStealthed",
    "IsMeditating",
    "IsInTotalDefense",
    "SetHealTarget",
    "GetHealTarget",
    "GetRandomDestination",
    "IsFormActive",
    "GetSpellFormMask",
    "GetSpellBaseForcePointCost",
    "SetKeepStealthInDialog",
    "HasLineOfSight",
    "ShowDemoScreen",
    "ForceHeartbeat",
    "EffectForceSight",
    "IsRunning",

    "SWMG_PlayerApplyForce",

    "SetForfeitConditions",
    "GetLastForfeitViolation",
    "ModifyReflexSavingThrowBase",
    "ModifyFortitudeSavingThrowBase",
    "ModifyWillSavingThrowBase",
    "GetScriptStringParameter",
    "GetObjectPersonalSpace",
    "AdjustCreatureAttributes",
    "SetCreatureAILevel",
    "ResetCreatureAILevel",
    "AddAvailablePUPByTemplate",
    "AddAvailablePUPByObject",
    "AssignPUP",
    "SpawnAvailablePUP",
    "AddPartyPuppet",
    "GetPUPOwner",
    "GetIsPuppet",
    "ActionFollowOwner",
    "GetIsPartyLeader",
    "GetPartyLeader",
    "RemoveNPCFromPartyToBase",
    "CreatureFlourishWeapon",
    "EffectMindTrick",
    "EffectFactionModifier",
    "ChangeObjectAppearance",
    "GetIsXBox",
    "EffectDroidScramble",
    "ActionSwitchWeapons",
    "PlayOverlayAnimation",
    "UnlockAllSongs",
    "DisableMap",
    "DetonateMine",
    "DisableHealthRegen",
    "SetCurrentForm",
    "SetDisableTransit",
    "SetInputClass",
    "SetForceAlwaysUpdate",
    "EnableRain",
    "DisplayMessageBox",
    "DisplayDatapad",
    "RemoveHeartbeat",
    "RemoveEffectByID",
    "RemoveEffectByExactMatch",
    "AdjustCreatureSkills",
    "GetSkillRankBase",
    "EnableRendering",
    "GetCombatActionsPending",
    "SaveNPCByObject",
    "SavePUPByObject",
    "GetIsPlayerMadeCharacter",
    "RebuildPartyTable"]


def is_valid_extract_dir(dir):
    return dir and os.path.isdir(dir)


root = tkinter.Tk()
root.withdraw()

extract_dir = os.getenv("REONE_EXTRACT_DIR")

if not is_valid_extract_dir(extract_dir):
    extract_dir = tkinter.filedialog.askdirectory(title="Choose extraction directory",
                                                  mustexist=True)
    if not is_valid_extract_dir(extract_dir):
        exit(1)

requested_module = input("Enter module name (optional): ")

calls_dict = {}

for pn in glob.glob("{}/**".format(extract_dir), recursive=True):
    _, ext = os.path.splitext(pn)
    if ext.lower() != ".pcode":
        continue
    modules_idx = pn.find("/modules/")
    if modules_idx != -1:
        ss = pn[modules_idx+9:]
        module = ss[:ss.find("/")]
        if requested_module and module.count(requested_module) == 0:
            continue
    with open(pn, "r") as f:
        lines = f.readlines()
        for line in lines:
            if not line.startswith("ACTION"):
                continue
            ss = line[7:]
            rn = ss[:ss.find(", ")]
            if not rn in calls_dict:
                calls_dict[rn] = 1
            else:
                calls_dict[rn] += 1


with open("routinecalls.txt", "w") as f:
    calls_list = [(rn, calls_dict[rn]) if rn in calls_dict else (rn, 0)
                  for rn in ROUTINES]
    sorted_calls = sorted(calls_list, key=lambda x: x[1], reverse=True)
    lines = map(lambda x: "{} {}\n".format(x[0], x[1]), sorted_calls)
    f.writelines(lines)
