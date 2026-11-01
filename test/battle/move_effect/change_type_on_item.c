#include "global.h"
#include "test/battle.h"

static uq4_12_t GetSpeciesTypeEffectiveness(enum Type moveType, enum Species species)
{
    enum Type type1 = GetSpeciesType(species, 0);
    enum Type type2 = GetSpeciesType(species, 1);
    uq4_12_t modifier = GetTypeModifier(moveType, type1);

    if (type1 != type2)
        modifier = uq4_12_multiply(modifier, GetTypeModifier(moveType, type2));

    return modifier;
}

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_TECHNO_BLAST) == EFFECT_CHANGE_TYPE_ON_ITEM);
    ASSUME(GetMoveEffectArg_HoldEffect(MOVE_TECHNO_BLAST) == HOLD_EFFECT_DRIVE);
    ASSUME(GetSpeciesTypeEffectiveness(TYPE_FIRE, SPECIES_VENUSAUR) == UQ_4_12(2.0));
    ASSUME(GetSpeciesTypeEffectiveness(TYPE_WATER, SPECIES_CHARIZARD) == UQ_4_12(2.0));
    ASSUME(GetSpeciesTypeEffectiveness(TYPE_ELECTRIC, SPECIES_BLASTOISE) == UQ_4_12(2.0));
    ASSUME(GetSpeciesTypeEffectiveness(TYPE_ICE, SPECIES_DRATINI) == UQ_4_12(2.0));
}

SINGLE_BATTLE_TEST("Techno Blast changes type depending on the drive the user holds")
{
    enum Species species;
    enum Item item;

    PARAMETRIZE { species = SPECIES_CHARIZARD; item = ITEM_DOUSE_DRIVE; }
    PARAMETRIZE { species = SPECIES_BLASTOISE; item = ITEM_SHOCK_DRIVE; }
    PARAMETRIZE { species = SPECIES_VENUSAUR; item = ITEM_BURN_DRIVE; }
    PARAMETRIZE { species = SPECIES_DRATINI; item = ITEM_CHILL_DRIVE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(item); }
        OPPONENT(species);
    } WHEN {
        TURN { MOVE(player, MOVE_TECHNO_BLAST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TECHNO_BLAST, player);
        EFFECTIVENESS_SE(opponent, SE_SUPER_EFFECTIVE);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Judgment changes type depending on the Plate the user holds")
{
    enum Species species;
    enum Item item;

    PARAMETRIZE { species = SPECIES_VENUSAUR; item = ITEM_FLAME_PLATE; }
    PARAMETRIZE { species = SPECIES_CHARIZARD; item = ITEM_SPLASH_PLATE; }
    PARAMETRIZE { species = SPECIES_BLASTOISE; item = ITEM_ZAP_PLATE; }
    PARAMETRIZE { species = SPECIES_DRATINI; item = ITEM_ICICLE_PLATE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_JUDGMENT) == EFFECT_CHANGE_TYPE_ON_ITEM);
        ASSUME(GetMoveEffectArg_HoldEffect(MOVE_JUDGMENT) == HOLD_EFFECT_PLATE);
        PLAYER(SPECIES_WOBBUFFET) { Item(item); }
        OPPONENT(species);
    } WHEN {
        TURN { MOVE(player, MOVE_JUDGMENT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_JUDGMENT, player);
        EFFECTIVENESS_SE(opponent, SE_SUPER_EFFECTIVE);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Legend Plate Judgment chooses Dragon against Palkia")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_LEGEND_PLATE].holdEffect == HOLD_EFFECT_PLATE);
        PLAYER(SPECIES_ARCEUS) { Item(ITEM_LEGEND_PLATE); }
        OPPONENT(SPECIES_PALKIA);
    } WHEN {
        TURN { MOVE(player, MOVE_JUDGMENT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_JUDGMENT, player);
        EFFECTIVENESS_SE(opponent, SE_SUPER_EFFECTIVE);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_NORMAL);
        EXPECT_EQ(player->types[1], TYPE_NORMAL);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Legend Plate Judgment chooses Dark against Giratina")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_LEGEND_PLATE].holdEffect == HOLD_EFFECT_PLATE);
        PLAYER(SPECIES_ARCEUS) { Item(ITEM_LEGEND_PLATE); }
        OPPONENT(SPECIES_GIRATINA);
    } WHEN {
        TURN { MOVE(player, MOVE_JUDGMENT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_JUDGMENT, player);
        EFFECTIVENESS_SE(opponent, SE_SUPER_EFFECTIVE);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_NORMAL);
        EXPECT_EQ(player->types[1], TYPE_NORMAL);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
    }
}

DOUBLE_BATTLE_TEST("Legend Plate Arceus returns to its normal type after all battlers act")
{
    GIVEN {
        PLAYER(SPECIES_ARCEUS) { Item(ITEM_LEGEND_PLATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PALKIA) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_JUDGMENT, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->types[0], TYPE_NORMAL);
        EXPECT_EQ(playerLeft->types[1], TYPE_NORMAL);
        EXPECT_EQ(playerLeft->types[2], TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Multi-Attack changes type depending on the Memory the user holds")
{
    enum Species species;
    enum Item item;

    PARAMETRIZE { species = SPECIES_VENUSAUR; item = ITEM_FIRE_MEMORY; }
    PARAMETRIZE { species = SPECIES_CHARIZARD; item = ITEM_WATER_MEMORY; }
    PARAMETRIZE { species = SPECIES_BLASTOISE; item = ITEM_ELECTRIC_MEMORY; }
    PARAMETRIZE { species = SPECIES_DRATINI; item = ITEM_ICE_MEMORY; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_MULTI_ATTACK) == EFFECT_CHANGE_TYPE_ON_ITEM);
        ASSUME(GetMoveEffectArg_HoldEffect(MOVE_MULTI_ATTACK) == HOLD_EFFECT_MEMORY);
        PLAYER(SPECIES_WOBBUFFET) { Item(item); }
        OPPONENT(species);
    } WHEN {
        TURN { MOVE(player, MOVE_MULTI_ATTACK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MULTI_ATTACK, player);
        EFFECTIVENESS_SE(opponent, SE_SUPER_EFFECTIVE);
        HP_BAR(opponent);
    }
}
