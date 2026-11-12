# Mining Minigame

This guide covers how to start and customize the Mining Minigame in this project. The feature is based on the [Mining Minigame guide](https://github.com/volromhacking/pokeemerald-expansion/wiki/Mining-Minigame), with steps adapted to this implementation.

## Starting the Minigame

The minigame can be launched from a script with the `StartMining` special. For example, add a script to a map and connect it to an object event:

```
MiningGame_EventScript_Start::
	lock
	faceplayer
	fadescreen FADE_TO_BLACK
	special StartMining
	release
	end
```

The `fadescreen` call is required before starting the minigame. When it ends, the player returns to the field and the script continues.

You can also launch it from the in-game debug menu: open the debug menu, choose **Utilities...**, then **Mining Minigame**.

## Configuration and Debug Options

Minigame IDs and layout constants are in [`include/constants/mining_minigame.h`](/include/constants/mining_minigame.h). Message-box and debug options are in [`include/config/mining_minigame.h`](/include/config/mining_minigame.h).

`MINING_FLAG_USE_DEFAULT_MESSAGE_BOX` selects which message box is used for minigame dialogue. It is `FALSE` by default, which uses the minigame's custom message box.

Debug options are disabled by default:

```c
#define MINING_DEBUG_ENABLE FALSE
```

To test item placement, temporarily enable debug mode and the item generation options:

```c
#define MINING_DEBUG_ENABLE                                 TRUE

#if MINING_DEBUG_ENABLE == TRUE
    #define MINING_DEBUG_ALL_SPRITES_VISIBLE                TRUE
    #define MINING_DEBUG_INFINITE_HITS                      FALSE
    #define MINING_DEBUG_ENABLE_ITEM_GENERATION_OPTIONS     TRUE
    #define MINING_DEBUG_ENABLE_STONE_GENERATION_OPTIONS    FALSE

    #if MINING_DEBUG_ENABLE_ITEM_GENERATION_OPTIONS == TRUE
        #define MINING_DEBUG_DESIRED_NUMBER_OF_ITEMS    2
        #define MINING_DEBUG_MININGID_ITEM1             MININGID_LIGHT_BALL
        #define MINING_DEBUG_MININGID_ITEM2             MININGID_LIGHT_BALL
        #define MINING_DEBUG_MININGID_ITEM3             MININGID_LIGHT_BALL
        #define MINING_DEBUG_MININGID_ITEM4             MININGID_LIGHT_BALL
    #endif

    #if MINING_DEBUG_ENABLE_STONE_GENERATION_OPTIONS == TRUE
        #define MINING_DEBUG_MININGID_STONE1            MININGID_STONE_MUSHROOM1
        #define MINING_DEBUG_MININGID_STONE2            MININGID_STONE_MUSHROOM2
    #endif
#endif
```

Replace `MININGID_LIGHT_BALL` with the mining ID you want to test. The requested item count is limited to four; a value of zero uses two items. `MINING_DEBUG_ALL_SPRITES_VISIBLE` reveals buried sprites for placement checks, `MINING_DEBUG_INFINITE_HITS` prevents the wall from collapsing, and the optional stone-generation settings choose the two stones. Turn `MINING_DEBUG_ENABLE` back off after testing.

## Adding an Item

The example below adds `ITEM_LIGHT_BALL` to the minigame. Use an item that already exists in your project; adding a new item to the bag requires adding it to the regular item data as well.

### 1. Add the sprite asset

Create a 64 by 64 pixel indexed PNG named `light_ball.png` in `graphics/mining_minigame/items/`. Use a 16-color palette and reserve palette index 0 for transparency. The build converts the PNG into the compressed sprite and palette used by the game.

### 2. Define its mining ID and sprite tag

In [`include/constants/mining_minigame.h`](/include/constants/mining_minigame.h), add a unique item sprite tag alongside the other item tags, before the stone tags:

```c
    MINING_TAG_ITEM_ARMOR_FOSSIL,
    MINING_TAG_ITEM_LIGHT_BALL,

    MINING_TAG_STONE_1X4,
```

Add a mining ID after the existing item IDs and before `MINING_COUNT_ID_STONE`:

```c
    MININGID_SKULL_FOSSIL,
    MININGID_ARMOR_FOSSIL,
    MININGID_LIGHT_BALL,
    MINING_COUNT_ID_STONE = MININGID_STONE_MUSHROOM2,
```

Keep the existing stone IDs and `MINING_COUNT_ID_STONE` assignment unchanged; the game uses that value when generating stones.

### 3. Register its graphics and bag item

In [`src/mining_minigame.c`](/src/mining_minigame.c), add the graphics declarations beside the other item graphics:

```c
static const u32 sItemLightBallGfx[] = INCGFX_U32("graphics/mining_minigame/items/light_ball.png", ".4bpp.smol");
static const u16 sItemLightBallPal[] = INCGFX_U16("graphics/mining_minigame/items/light_ball.png", ".gbapal");
```

Add a compressed sprite sheet alongside the other item sheets:

```c
static const struct CompressedSpriteSheet sSpriteSheet_ItemLightBall =
{
    sItemLightBallGfx,
    2048,
    MINING_TAG_ITEM_LIGHT_BALL,
};
```

The 2048-byte size is for a 64 by 64, 4bpp sprite. Add an entry to `sMiningItemList` mapping the mining ID to the actual bag item, sprite tag, sheet, and palette:

```c
    [MININGID_LIGHT_BALL] =
    {
        .bagItemId = ITEM_LIGHT_BALL,
        .tag = MINING_TAG_ITEM_LIGHT_BALL,
        .sheet = &sSpriteSheet_ItemLightBall,
        .paldata = sItemLightBallPal,
    },
```

This mapping is what lets the minigame display and award the corresponding bag item.

### 4. Register the sprite for transparency analysis

Add the generated 4bpp asset to `items` in [`tools/mining_minigame/table.json`](/tools/mining_minigame/table.json):

```json
{
    "id": "MININGID_LIGHT_BALL",
    "path": "./graphics/mining_minigame/items/light_ball.4bpp"
}
```

Add it as another entry in the existing `items` array; keep the surrounding JSON commas valid. This table lets the sprite analyzer match the converted graphics to the mining ID and calculate which sprite tiles contain visible pixels.

Do not edit `src/data/mining_minigame.h` directly. It is generated by `tools/mining_minigame/analyze_sprites.py` during the build from the 4bpp graphics and `table.json`.

### 5. Add it to the random item pool

In `src/mining_minigame.c`, add the ID to one of `sItemRarityTable_Common`, `sItemRarityTable_Uncommon`, or `sItemRarityTable_Rare`. For example, to make the Light Ball rare:

```c
static const enum MiningId sItemRarityTable_Rare[] =
{
    MININGID_STAR_PIECE,
    /* Existing rare items... */
    MININGID_LIGHT_BALL,
};
```

Items that are registered in `sMiningItemList` but omitted from all rarity tables can still be selected using debug item generation, but will not appear during normal random generation. The game selects the common, uncommon, or rare category with relative weights of 4:2:1, then chooses uniformly from that category.

### 6. Build and test

From the repository root, run `make` so the asset conversions and generated mining data are refreshed. To check the new item deterministically, enable `MINING_DEBUG_ENABLE` and `MINING_DEBUG_ENABLE_ITEM_GENERATION_OPTIONS` in `include/config/mining_minigame.h`, select the new ID in the debug item slots, then start the minigame from the debug menu or your map script. Dig up the item to confirm it is added to the bag. Disable debug mode when finished.

## Adding Other Items

Repeat the same steps for each item: add its PNG, unique mining ID and sprite tag, graphics and sprite-sheet declarations, `sMiningItemList` mapping, `table.json` entry, and rarity-table entry. If two sprites use the same palette, the palette pointer may be shared; the fossil sprites share `sItemFossilPal` in the existing implementation.

The supplied Dream sprites are also registered as rare finds. Their custom sprite names map to the existing bag items:

| Sprite | Bag item |
|---|---|
| Dream Ice Stone | `ITEM_ICE_STONE` |
| Dream Cover Fossil | `ITEM_COVER_FOSSIL` |
| Dream Plume Fossil | `ITEM_PLUME_FOSSIL` |
| Dream Jaw Fossil | `ITEM_JAW_FOSSIL` |
| Dream Sail Fossil | `ITEM_SAIL_FOSSIL` |
| Dream Shiny Stone | `ITEM_SHINY_STONE` |
| Dream Dusk Stone | `ITEM_DUSK_STONE` |
| Dream Dawn Stone | `ITEM_DAWN_STONE` |
