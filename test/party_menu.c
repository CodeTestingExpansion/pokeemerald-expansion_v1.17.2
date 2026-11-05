#include "config/party_menu.h"
#include "global.h"
#include "battle.h"
#include "party_menu.h"
#include "pokemon.h"
#include "test/test.h"

#define TEST_MENU_DIR_DOWN     1
#define TEST_MENU_DIR_UP      -1
#define TEST_MENU_DIR_RIGHT    2
#define TEST_MENU_DIR_LEFT    -2

static void SetTestPartySize(enum BattleTrainer trainer, u8 partySize)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[trainer][i]);

    for (u32 i = 0; i < partySize; i++)
        CreateMon(&gParties[trainer][i], SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PRESET(0));

    gPartiesCount[trainer] = partySize;
}

TEST("Full multi partner party menu stops down navigation at partner party count")
{
    SetTestPartySize(B_TRAINER_PLAYER, PARTY_SIZE);
    SetTestPartySize(B_TRAINER_PARTNER, 2);
    gPartyMenu.layout = PARTY_LAYOUT_MULTI_FULL_PARTNER;

    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(1, TEST_MENU_DIR_DOWN, FALSE, 0), PARTY_SIZE + 1);
}

TEST("Full multi partner party menu allows down navigation through partner party count")
{
    SetTestPartySize(B_TRAINER_PLAYER, 2);
    SetTestPartySize(B_TRAINER_PARTNER, PARTY_SIZE);
    gPartyMenu.layout = PARTY_LAYOUT_MULTI_FULL_PARTNER;

    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(1, TEST_MENU_DIR_DOWN, FALSE, 0), 2);
}

TEST("Full multi partner party menu wraps cancel up to partner party count")
{
    SetTestPartySize(B_TRAINER_PLAYER, PARTY_SIZE);
    SetTestPartySize(B_TRAINER_PARTNER, 2);
    gPartyMenu.layout = PARTY_LAYOUT_MULTI_FULL_PARTNER;

    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(PARTY_SIZE + 1, TEST_MENU_DIR_UP, FALSE, 0), 1);
}

#if PARTY_MENU_GEN5ISH_LAYOUT
static s8 UpdateSinglePartyMenuSelection(s8 slotId, s8 direction, u8 *lastSelectedSlot)
{
    return Test_UpdatePartySelectionSingleLayoutWithLastSelectedSlot(slotId, direction, FALSE, lastSelectedSlot);
}

TEST("Single party menu wraps down independently within each column")
{
    SetTestPartySize(B_TRAINER_PLAYER, PARTY_SIZE);
    gPartyMenu.layout = PARTY_LAYOUT_SINGLE;
    u8 lastSelectedSlot = 0;

    EXPECT_EQ(UpdateSinglePartyMenuSelection(0, TEST_MENU_DIR_DOWN, &lastSelectedSlot), 2);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(2, TEST_MENU_DIR_DOWN, &lastSelectedSlot), 4);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(4, TEST_MENU_DIR_DOWN, &lastSelectedSlot), PARTY_SIZE + 1);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(PARTY_SIZE + 1, TEST_MENU_DIR_DOWN, &lastSelectedSlot), 0);

    EXPECT_EQ(UpdateSinglePartyMenuSelection(1, TEST_MENU_DIR_DOWN, &lastSelectedSlot), 3);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(3, TEST_MENU_DIR_DOWN, &lastSelectedSlot), 5);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(5, TEST_MENU_DIR_DOWN, &lastSelectedSlot), PARTY_SIZE + 1);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(PARTY_SIZE + 1, TEST_MENU_DIR_DOWN, &lastSelectedSlot), 1);
}

TEST("Single party menu wraps up independently within each column")
{
    SetTestPartySize(B_TRAINER_PLAYER, PARTY_SIZE);
    gPartyMenu.layout = PARTY_LAYOUT_SINGLE;
    u8 lastSelectedSlot = 0;

    EXPECT_EQ(UpdateSinglePartyMenuSelection(4, TEST_MENU_DIR_UP, &lastSelectedSlot), 2);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(2, TEST_MENU_DIR_UP, &lastSelectedSlot), 0);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(0, TEST_MENU_DIR_UP, &lastSelectedSlot), PARTY_SIZE + 1);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(PARTY_SIZE + 1, TEST_MENU_DIR_UP, &lastSelectedSlot), 4);

    EXPECT_EQ(UpdateSinglePartyMenuSelection(5, TEST_MENU_DIR_UP, &lastSelectedSlot), 3);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(3, TEST_MENU_DIR_UP, &lastSelectedSlot), 1);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(1, TEST_MENU_DIR_UP, &lastSelectedSlot), PARTY_SIZE + 1);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(PARTY_SIZE + 1, TEST_MENU_DIR_UP, &lastSelectedSlot), 5);
}

TEST("Single party menu moves horizontally between mons in each row")
{
    SetTestPartySize(B_TRAINER_PLAYER, PARTY_SIZE);
    gPartyMenu.layout = PARTY_LAYOUT_SINGLE;

    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(0, TEST_MENU_DIR_RIGHT, FALSE, 0), 1);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(1, TEST_MENU_DIR_RIGHT, FALSE, 0), 0);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(0, TEST_MENU_DIR_LEFT, FALSE, 0), 1);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(1, TEST_MENU_DIR_LEFT, FALSE, 0), 0);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(2, TEST_MENU_DIR_RIGHT, FALSE, 0), 3);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(3, TEST_MENU_DIR_LEFT, FALSE, 0), 2);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(4, TEST_MENU_DIR_RIGHT, FALSE, 0), 5);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(5, TEST_MENU_DIR_LEFT, FALSE, 0), 4);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(PARTY_SIZE + 1, TEST_MENU_DIR_LEFT, FALSE, 0), PARTY_SIZE + 1);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(PARTY_SIZE + 1, TEST_MENU_DIR_RIGHT, FALSE, 0), PARTY_SIZE + 1);
}

TEST("Single party menu navigates a partially filled final row")
{
    SetTestPartySize(B_TRAINER_PLAYER, 5);
    gPartyMenu.layout = PARTY_LAYOUT_SINGLE;
    u8 lastSelectedSlot = 0;

    EXPECT_EQ(UpdateSinglePartyMenuSelection(1, TEST_MENU_DIR_DOWN, &lastSelectedSlot), 3);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(3, TEST_MENU_DIR_DOWN, &lastSelectedSlot), PARTY_SIZE + 1);
    EXPECT_EQ(lastSelectedSlot & 1, 1);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(PARTY_SIZE + 1, TEST_MENU_DIR_DOWN, &lastSelectedSlot), 1);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(3, TEST_MENU_DIR_UP, &lastSelectedSlot), 1);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(1, TEST_MENU_DIR_UP, &lastSelectedSlot), PARTY_SIZE + 1);
    EXPECT_EQ(UpdateSinglePartyMenuSelection(PARTY_SIZE + 1, TEST_MENU_DIR_UP, &lastSelectedSlot), 3);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(4, TEST_MENU_DIR_DOWN, FALSE, 0), PARTY_SIZE + 1);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(PARTY_SIZE + 1, TEST_MENU_DIR_UP, FALSE, 0), 4);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(4, TEST_MENU_DIR_UP, FALSE, 0), 2);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(0, TEST_MENU_DIR_RIGHT, FALSE, 0), 1);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(4, TEST_MENU_DIR_RIGHT, FALSE, 0), 4);
}
#else
TEST("Vanilla single party menu keeps vertical navigation")
{
    SetTestPartySize(B_TRAINER_PLAYER, PARTY_SIZE);
    gPartyMenu.layout = PARTY_LAYOUT_SINGLE;

    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(0, TEST_MENU_DIR_DOWN, FALSE, 0), 1);
    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(0, TEST_MENU_DIR_RIGHT, FALSE, 0), 1);
}
#endif
