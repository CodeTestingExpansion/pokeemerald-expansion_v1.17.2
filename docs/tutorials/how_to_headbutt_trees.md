# How to add Headbutt Trees and Encounters

The overworld Headbutt feature lets the player interact with trees and encounter wild Pokémon. This guide explains how to set up a tree on a map and define that map's encounter table.

## Add a headbuttable tree to a map

1. Open the map in Porymap.
2. Select the tree metatile that should trigger Headbutt.
3. Set that metatile's behavior to `MB_HEADBUTT`.
4. Make sure the player can stand next to the tree and face it.

The behavior automatically starts the Headbutt interaction script. No separate map object or script is needed. The player can also choose a Pokémon that knows Headbutt from the party menu while facing an `MB_HEADBUTT` metatile.

## Add Pokémon to the tree encounters

Edit `src/data/wild_encounters.json` and find the entry for the map containing the tree. Add a `headbutt_mons` table to that map entry:

```json
{
  "headbutt_mons": {
    "encounter_rate": 20,
    "mons": [
      { "min_level": 5, "max_level": 5, "species": "SPECIES_POOCHYENA" },
      { "min_level": 5, "max_level": 5, "species": "SPECIES_TAILLOW" },
      { "min_level": 6, "max_level": 6, "species": "SPECIES_SEEDOT" },
      { "min_level": 6, "max_level": 6, "species": "SPECIES_NINCADA" },
      { "min_level": 7, "max_level": 7, "species": "SPECIES_SLAKOTH" }
    ]
  }
}
```

Headbutt uses five encounter slots. The `headbutt_mons` field in the `wild_encounter_groups` section configures their default weights as `60, 30, 5, 4, 1`; change those weights there if desired. Each map's table must contain exactly five Pokémon in the same slot order. `encounter_rate` controls the chance that Headbutt finds a Pokémon; the level and species in each slot define which Pokémon can appear.

Omit `headbutt_mons` from maps where no Headbutt encounters should occur. After editing the JSON, rebuild so the wild encounter data is regenerated.

## Test it

Build the game, load the map, face the tree, and interact with it. Confirm the Headbutt prompt appears and that an encounter can be started. You can also use Headbutt from the party menu while facing the tree.
