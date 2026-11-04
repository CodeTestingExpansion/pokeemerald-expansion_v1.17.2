#ifndef GUARD_UI_STAT_EDITOR_H
#define GUARD_UI_STAT_EDITOR_H

#include "global.h"
#include "main.h"

#if STAT_EDITOR_START_MENU || STAT_EDITOR_PARTY_MENU
void Task_OpenStatEditorFromStartMenu(u8 taskId);
void StatEditor_Init(u8 partyId, MainCallback callback);
#endif

#endif // GUARD_UI_STAT_EDITOR_H
