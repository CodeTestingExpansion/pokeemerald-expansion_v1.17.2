#ifndef GUARD_QUESTS_H
#define GUARD_QUESTS_H

#include "config/overworld.h"
#include "main.h"

#if QUEST_MENU_TYPE == QUEST_MENU_1
#define NO_ACTIVE_QUEST -1

struct SideQuest
{
	/*0x00*/ const u8* name;
	/*0x04*/ const u8* desc;
	/*0x08*/ const u8* poc;
	/*0x0C*/ const u8* map;
	///*0x10*/ const u8* hint;
	/*0x14*/ const u8* reward;
}; /* size = 0x18 */


extern const struct SideQuest gSideQuests[SIDE_QUEST_COUNT];

enum
{
	QUEST_DIFFICULTY_EASY,
	QUEST_DIFFICULTY_MEDIUM,
	QUEST_DIFFICULTY_HARD,
	QUEST_DIFFICULTY_EXTREME,
};

enum QuestCases
{
    FLAG_GET_UNLOCKED,      // check if quest is unlocked
    FLAG_SET_UNLOCKED,      // mark unlocked quest
    FLAG_GET_COMPLETED,     // check if quest is completed
    FLAG_SET_COMPLETED,     // mark completed quest
};

// functions
void CreateItemMenuIcon(u16, u8);
void ResetItemMenuIconState(void);
void DestroyItemMenuIcon(u8 idx);
void Task_OpenQuestMenuFromStartMenu(u8);
s8 GetSetQuestFlag(u8 quest, u8 caseId);
s8 GetActiveQuestIndex(void);
void SetActiveQuest(u8 questId);
void TextWindow_SetStdFrame0_WithPal(u8 windowId, u16 destOffset, u8 palIdx);
void QuestMenu_Init(u8 a0, MainCallback callback);
void SetQuestMenuActive(void);
void ResetActiveQuest(void);
void CopyQuestName(u8 *dst, u8 questId);

#elif QUEST_MENU_TYPE == QUEST_MENU_2

#define SORT_DEFAULT 0
#define SORT_INACTIVE 1
#define SORT_ACTIVE 2
#define SORT_REWARD 3
#define SORT_DONE 4
#define SORT_DEFAULT_AZ 10
#define SORT_INACTIVE_AZ 11
#define SORT_ACTIVE_AZ 12
#define SORT_REWARD_AZ 13
#define SORT_DONE_AZ 14
#define SORT_SUBQUEST 100

#define INCREMENT 1
#define ALPHA 2
#define SUB 3

#define OBJECT 1
#define ITEM 2
#define PKMN 3

struct SubQuest
{
    const u8 id;
    const u8 *name;
    const u8 *desc;
    const u8 *map;
    const u16 sprite;
    const u8 spritetype;
    const u8 *type;
};

struct SideQuest
{
    const u8 *name;
    const u8 *desc;
    const u8 *donedesc;
    const u8 *map;
    const u16 sprite;
    const u8 spritetype;
    const struct SubQuest *subquests;
    const u8 numSubquests;
};

enum QuestCases
{
    FLAG_GET_UNLOCKED,
    FLAG_GET_INACTIVE,
    FLAG_GET_ACTIVE,
    FLAG_GET_REWARD,
    FLAG_GET_COMPLETED,
    FLAG_GET_FAVORITE,
    FLAG_SET_UNLOCKED,
    FLAG_SET_INACTIVE,
    FLAG_SET_ACTIVE,
    FLAG_SET_REWARD,
    FLAG_SET_COMPLETED,
    FLAG_SET_FAVORITE,
    FLAG_REMOVE_INACTIVE,
    FLAG_REMOVE_ACTIVE,
    FLAG_REMOVE_REWARD,
    FLAG_REMOVE_FAVORITE,
};

void QuestMenu_Init(u8 a0, MainCallback callback);
u8 QuestMenu_GetSetSubquestState(u8 quest, u8 caseId, u8 childQuest);
u8 QuestMenu_GetSetQuestState(u8 quest, u8 caseId);
void Task_QuestMenu_OpenFromStartMenu(u8 taskId);
void QuestMenu_CopyQuestName(u8 *dst, u8 questId);
void QuestMenu_CopySubquestName(u8 *dst, u8 parentId, u8 childId);
void QuestMenu_ResetMenuSaveData(void);

#endif

#endif // GUARD_QUESTS_H
