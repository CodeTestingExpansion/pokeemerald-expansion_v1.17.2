# Quest Menu: Setup and Quest Authoring

This guide covers the two quest-menu implementations available in this project:

- **Ghoulslash** (`QUEST_MENU_1`): quests with unlocked, active, and completed states.
- **Unbound** (`QUEST_MENU_2`): quests with inactive, active, reward, completed, and favorite states, plus subquests.

Only one implementation is compiled at a time.

## 1. Select and expose the menu

Set `QUEST_MENU_TYPE` in `include/config/overworld.h`:

```c
#define QUEST_MENU_TYPE QUEST_MENU_1 // Ghoulslash
```

or:

```c
#define QUEST_MENU_TYPE QUEST_MENU_2 // Unbound
```

`QUEST_MENU_0` disables the quest menu. Rebuild the ROM after changing this setting. The two implementations use different save data layouts, so use a new game or back up your save before switching modes.

To show **QUESTS** in the Start Menu, set the system flag from an event script:

```asm
EventScript_EnableQuestMenu::
    setflag FLAG_SYS_QUEST_MENU_GET
    return
```

Alternatively, open the menu directly from a script:

```asm
EventScript_OpenQuestMenu::
    openquestmenu
    end
```

The `openquestmenu` macro is shorthand for `questmenu QUEST_MENU_OPEN, 0`. In Ghoulslash mode, opening the menu also sets `FLAG_QUEST_MENU_ACTIVE`; `FLAG_SYS_QUEST_MENU_GET` is the explicit Start Menu unlock used by both modes.

## 2. Add quest text and data

Quest IDs are zero-based in scripts: the first quest is ID `0`, the second is ID `1`, and so on. Use the named constants rather than numeric literals. Keep each quest's text and data table entry aligned with its ID.

### Ghoulslash

Edit the quest table in `src/quests.c`. Each entry supplies a name, description, point of contact, map/location text, and reward:

```c
side_quest(gText_SideQuestName_1,
           gText_SideQuestDesc_1,
           gText_SideQuestPOC_1,
           gText_SideQuestMap_1,
           gText_SideQuestReward_1),
```

Edit or add the corresponding strings in `src/strings.c`; declarations belong in the Ghoulslash section of `include/strings.h`. The imported entries are example placeholders such as “Side Quest 1” and “Description 1.” Replace them with your game's text. The table currently has 30 slots, `SIDE_QUEST_1` through `SIDE_QUEST_30`.

### Unbound

Edit the `sSideQuests` table in `src/quests_unbound.c`. Each entry supplies a name, description, completed description, location, icon type/data, and optional subquest array:

```c
side_quest(
    gText_SideQuestName_1,
    gText_SideQuestDesc_1,
    gText_SideQuestDoneDesc_1,
    gText_SideQuestMap1,
    OBJ_EVENT_GFX_WALLY,
    OBJECT,
    NULL,
    0
),
```

Replace the example text in `src/quests_unbound_strings.c`. Declarations for Unbound text are in the `QUEST_MENU_2` section of `include/strings.h`. For the icon fields, `OBJECT` uses an object graphics ID, `ITEM` uses an item ID, and `PKMN` uses a species ID.

The Unbound table also has 30 quest slots, `QUEST_1` through `QUEST_30`.

The table entry order must match the zero-based quest ID constants in `include/constants/quests_unbound.h`. To add a parent quest, add the next ID, update `QUEST_COUNT`, add its name/description/completed-description/location strings and declarations, and append the matching `side_quest` entry to `sSideQuests`. A quest's active description is shown while it is active; the completed description is shown after completion. Inactive and reward-ready quests use the menu's built-in prompts.

The current quest and subquest counts are used to size both the menu's data and the quest save fields. If you expand them, update the count expressions in `include/constants/quests_unbound.h` and keep the number of table entries aligned. The SaveBlock2 layout changes when these counts change; use a backed-up save or start a new game, and ensure the resulting save block still fits the project's limits.

## 3. Unlock and change quest state

Use the quest command macros in event scripts. The quest ID argument is encoded as a byte and is intended to be a quest constant/immediate; do not pass a `VAR_0x800x` variable as this argument.

### Ghoulslash state commands

```asm
    questmenu QUEST_MENU_UNLOCK_QUEST, SIDE_QUEST_1
    questmenu QUEST_MENU_SET_ACTIVE, SIDE_QUEST_1
```

When the objective is finished:

```asm
    questmenu QUEST_MENU_COMPLETE_QUEST, SIDE_QUEST_1
    questmenu QUEST_MENU_RESET_ACTIVE, 0
```

The available operations are `QUEST_MENU_UNLOCK_QUEST`, `QUEST_MENU_SET_ACTIVE`, `QUEST_MENU_COMPLETE_QUEST`, `QUEST_MENU_RESET_ACTIVE`, `QUEST_MENU_CHECK_UNLOCKED`, and `QUEST_MENU_CHECK_COMPLETE`. Ghoulslash stores one active quest at a time. It does not have a reward-ready state or subquests.

### Unbound state commands

```asm
    questmenu QUEST_MENU_UNLOCK_QUEST, QUEST_1
    questmenu QUEST_MENU_SET_ACTIVE, QUEST_1
```

When the objective is ready for turn-in, mark the reward available:

```asm
    questmenu QUEST_MENU_SET_REWARD, QUEST_1
```

After the reward or final conversation has been delivered:

```asm
    questmenu QUEST_MENU_COMPLETE_QUEST, QUEST_1
```

Setting a quest active also unlocks it. Setting it to reward removes its active state; completing it removes its active and reward states. Other commands include `QUEST_MENU_CHECK_UNLOCKED`, `QUEST_MENU_CHECK_INACTIVE`, `QUEST_MENU_CHECK_ACTIVE`, `QUEST_MENU_CHECK_REWARD`, and `QUEST_MENU_CHECK_COMPLETE`.

The command constants differ between modes. Use the constants from the selected mode and do not hard-code their numeric values.

### Unbound menu controls and name helpers

In the Unbound menu, the player can:

- Press **R** to cycle between the inactive, active, reward-ready, and completed quest filters. Filtering is not available in a subquest list.
- Press **Start** to toggle alphabetical ordering for the current quest list.
- Press **Select** on a quest to favorite or unfavorite it. Favorites are saved and appear at the top of the main quest list.
- Press **A** on a quest with children to view its subquests; **B** returns to the parent list.

Sorting and filtering are menu-session choices; favorites persist. These controls do not apply while viewing subquests. List wrapping is controlled separately by `QUEST_MENU_WRAP_LIST` in `include/config/overworld.h`.

Unbound scripts can also put a quest or subquest name into `{STR_VAR_1}` for dialogue:

```asm
    questmenu QUEST_MENU_BUFFER_QUEST_NAME, QUEST_1
    msgbox Text_QuestName, MSGBOX_DEFAULT

Text_QuestName::
    .string "{STR_VAR_1}$"
```

For a subquest, use `subquestmenu QUEST_MENU_BUFFER_QUEST_NAME, QUEST_1, SUB_QUEST_1`. `returnqueststate QUEST_1` writes the current state to `VAR_RESULT`: `1` inactive, `2` active, `3` reward-ready, or `4` completed. Use the explicit `QUEST_MENU_CHECK_*` commands when a boolean condition is all you need.

## 4. Track objectives in event scripts

Quest state does not automatically monitor game events. Put a check or completion command in the event that represents the objective: an NPC, item-ball script, post-battle script, or another map event. The examples below use `QUEST_1` for Unbound and `SIDE_QUEST_1` for Ghoulslash; substitute the ID for your quest.

For Unbound, an objective can either finish the quest immediately or set it to reward-ready, with a later NPC interaction completing it. For Ghoulslash, complete the quest at the objective, or use a separate event flag to remember that it is ready for turn-in.

### See a species in the wild

The `getseenmon` macro reads the species' Pokédex Seen flag into `VAR_RESULT`:

```asm
EventScript_CheckPikachuSeen::
    getseenmon SPECIES_PIKACHU
    goto_if_eq VAR_RESULT, FALSE, EventScript_PikachuNotSeen
    @ Mark the objective done here.
    return

EventScript_PikachuNotSeen::
    return
```

This is a **global species check**, not a record of where the Pokémon was seen. Seeing that species anywhere satisfies the check. It is also checked only when this script runs; the wild encounter itself does not run this quest script. For a route/location-specific objective, run the check from an event on that route and add a separate map-specific flag or scripted encounter hook if the quest must prove the encounter happened there.

### Catch a species

Use `getcaughtmon` to check the Pokédex Caught flag:

```asm
EventScript_CheckCaughtPikachu::
    getcaughtmon SPECIES_PIKACHU
    goto_if_eq VAR_RESULT, FALSE, EventScript_PikachuNotCaught
    @ Mark the objective done here.
    return

EventScript_PikachuNotCaught::
    return
```

This check still succeeds if the Pokémon has been moved out of the party. Use `checkspecies SPECIES_PIKACHU` instead when the requirement is specifically to have that species in the current party.

### Find an item

For an overworld item ball, use `finditem`. It gives the item and removes the last-talked-to item object when successful. Check bag space first so a full bag does not accidentally count as a completed objective:

```asm
EventScript_QuestItemBall::
    checkitemspace ITEM_BIG_PEARL
    goto_if_eq VAR_RESULT, FALSE, EventScript_QuestItemBagFull
    finditem ITEM_BIG_PEARL
    @ The space check succeeded, so the item pickup completed.
    @ Mark the objective done here.
    end

EventScript_QuestItemBagFull::
    lock
    faceplayer
    msgbox Text_QuestItemBagFull, MSGBOX_DEFAULT
    release
    end
```

Assign this script to the quest's item-ball object. `finditem` handles the pickup dialogue and item-ball removal; do not also call `removeobject` for the same object. The standard shared item-ball script cannot be changed to complete only one quest, so use a quest-specific script for this item.
Define `Text_QuestItemBagFull` with the message you want to show when the bag has no room.

For a hidden item or other custom reward, put the state update after the command that actually awards the item, and only on its success path.

### Deliver an item

Place this check in the recipient NPC's script. The item is consumed only if the player has the requested quantity:

```asm
EventScript_DeliverPearls::
    checkitem ITEM_BIG_PEARL, 3
    goto_if_eq VAR_RESULT, FALSE, EventScript_NeedMorePearls
    removeitem ITEM_BIG_PEARL, 3
    goto_if_eq VAR_RESULT, FALSE, EventScript_NeedMorePearls
    @ Give the delivery dialogue/reward.
    @ Mark the quest complete here.
    return

EventScript_NeedMorePearls::
    @ Explain that the player needs three Pearls.
    return
```

The second check handles the removal result explicitly. Add your mode's state commands on the successful-delivery path only.

### Talk to an NPC

If talking to the NPC is the objective, put the completion command in that NPC's event script after any required choice or dialogue:

```asm
EventScript_QuestContact::
    lock
    faceplayer
    msgbox Text_QuestContact, MSGBOX_DEFAULT
    @ Mark the objective done here.
    release
    end
```

For a multi-step conversation, only mark completion after the player accepts or satisfies the required condition. Use an event flag to prevent a one-time interaction from repeating its reward or dialogue.

### Defeat an NPC trainer

Use the optional post-battle event script argument of `trainerbattle_single` to update the quest after the trainer is defeated:

```asm
EventScript_QuestTrainer::
    trainerbattle_single TRAINER_HIKER_TEST, Text_TrainerIntro, Text_TrainerDefeat, EventScript_QuestTrainerDefeated
    return

EventScript_QuestTrainerDefeated::
    @ Mark the objective done here.
    return
```

If the trainer already has a post-battle script, add the quest update to that script instead of replacing its existing story, badge, or trainer-flag behavior. For an objective that requires only talking to the trainer, update the quest from the regular interaction script rather than the post-battle script.

### Other objective patterns

The same approach works for other events:

- **Visit a place:** put the check in a map entry/trigger or sign script; set a map-specific flag if revisiting must not repeat the objective.
- **Collect several things:** give each pickup an Unbound subquest, or maintain a counter in a game variable and check the threshold before setting the parent quest to reward-ready.
- **Use an item or field move:** place the update after the item/move succeeds, not merely when it is selected.
- **Complete a story milestone:** check the relevant story flag or trainer flag in the turn-in script.
- **Have a Pokémon in the party:** use `checkspecies`; use `getcaughtmon` when party membership is not required.

In all cases, make the update idempotent where possible: a completed quest or subquest should not grant its reward repeatedly.

## 5. Add Unbound subquests

Subquests exist only in Unbound mode. Define each subquest in one of the arrays near the top of `src/quests_unbound.c`, then reference that array from its parent in `sSideQuests`:

```c
#define MY_QUEST_SUB_COUNT 1
static const struct SubQuest sMyQuestSubquests[MY_QUEST_SUB_COUNT] =
{
    sub_quest(
        0, // Use a save-bit ID not used by another subquest.
        gText_SubQuest1_Name1,
        gText_SubQuest1_Desc1,
        gText_SideQuestMap1,
        OBJ_EVENT_GFX_WALLY,
        OBJECT,
        sText_Found
    ),
};
```

Set the parent quest's `subquests` pointer and `numSubquests` to this array and its actual number of entries:

```c
side_quest(
    gText_SideQuestName_1,
    gText_SideQuestDesc_1,
    gText_SideQuestDoneDesc_1,
    gText_SideQuestMap1,
    OBJ_EVENT_GFX_WALLY,
    OBJECT,
    sMyQuestSubquests,
    MY_QUEST_SUB_COUNT
),
```

Add the subquest name and description in `src/quests_unbound_strings.c`, with declarations in `include/strings.h`. Set the array size and the parent's `numSubquests` to the actual number of entries. The array's order is significant: `subquestmenu` takes the **zero-based index within that parent's array**. The first child is index `0`, even when its `.id` save-bit value is `10` or higher. Each `.id` must instead be unique across all subquests, and `SUB_QUEST_COUNT` must cover the highest `.id` plus one. The imported example already uses IDs `0` through `29`; when extending it, reassign IDs as needed to avoid collisions. The current configuration reserves 30 subquest IDs; if you increase the total, also update `SUB_QUEST_COUNT` and the SaveBlock2 sizing.

To add more children, declare a count for the parent in `include/constants/quests_unbound.h`, include that count in `SUB_QUEST_COUNT`, and add one matching `SubQuest` entry per child. Add the corresponding `SUB_QUEST_n` constants for child indices used by scripts. Keep the child index (which selects an entry in one parent's array) distinct from the entry's globally unique `.id` (which selects its saved completion bit). As with parent quest expansion, increasing these counts changes SaveBlock2 layout.

Mark a child complete from its objective event:

```asm
    subquestmenu QUEST_MENU_COMPLETE_QUEST, QUEST_1, SUB_QUEST_1
```

Check a child's state with:

```asm
    subquestmenu QUEST_MENU_CHECK_COMPLETE, QUEST_1, SUB_QUEST_1
    goto_if_eq VAR_RESULT, FALSE, EventScript_SubquestNotDone
```

Once all required children are complete, explicitly move the parent quest to reward-ready. The menu does not automatically complete or advance the parent when its children are done. For example, check each required child in the turn-in NPC script, then call:

```asm
    questmenu QUEST_MENU_SET_REWARD, QUEST_1
```

After giving the final reward, call `questmenu QUEST_MENU_COMPLETE_QUEST, QUEST_1`.

## 6. Quick checklist

1. Select the implementation in `include/config/overworld.h` and rebuild.
2. Set `FLAG_SYS_QUEST_MENU_GET` to show the Start Menu entry.
3. Add quest text and the matching data-table entry.
4. Unlock and activate the quest from a script.
5. Add objective checks to the event that actually proves completion.
6. In Unbound mode, define subquest arrays and explicitly transition the parent state.
7. Test bag-full, incomplete-objective, repeated-interaction, and successful turn-in paths.

For further background on the Unbound menu's states, filters, and controls, see the [Unbound Quest Menu wiki](https://github.com/PokemonSanFran/pokeemerald/wiki/Unbound-Quest-Menu).
