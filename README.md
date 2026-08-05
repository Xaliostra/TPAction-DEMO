# Targeted Limb Combat Prototype

A UE5 C++ technical gameplay study exploring targeted limb combat in a third-person melee game.

Inspired by the design problem of readable limb targeting in action games.
This is an original prototype, not a recreation of any existing game.

[Gameplay video](...) · [Portfolio](...)

![Gameplay GIF: lock-on, limb selection, hit, dismemberment]
<img width="334" height="216" alt="TPAction_Target" src="https://github.com/user-attachments/assets/37246ae5-befc-4798-8135-042da6e68d6d" />


## Project Goal

To build a readable combat loop in which the player can:

1. lock onto an enemy;
2. select a specific limb;
3. attack using animation-driven weapon traces;
4. apply damage and armor to the selected hit limb;
5. receive visual/UI feedback when a limb is damaged or destroyed.

## Features

- Lock-on targeting with camera adjustment
- Screen-space limb selection UI
- Limb-specific health, armor, and damage multipliers
- Bone-to-limb mapping for hit detection
- Animation-notify-driven weapon tracing
- Tag-driven trace socket selection per attack section
- GAS-based melee combo and dodge abilities
- Motion Warping-based directional dodge
- Limb destruction with mesh hiding, wound mesh, detached physics limb, and UI updates
- Basic enemy combat loop using Behavior Trees and GAS abilities

## Combat Pipeline

```mermaid 
flowchart LR
    A["Player input"] --> B["GAS attack ability"]
    B --> C["Animation montage / notify"]
    C --> D["Weapon socket trace"]
    D --> E["Hit result + bone"]
    E --> F["Limb mapping"]
    F --> G["Damage calculation: armor + multiplier"]
    G --> H["Limb health / destruction"]
    H --> I["HUD and visual feedback"]
```

## Technical Highlights

Limb damage
- Skeletal Mesh bones are cached into tag driven map for O(1) hit resolution
- Dismembered limb remains, but is hidden for both visuals and physics
- Dismembered limb gets replaced by the wound mesh in the root-for-limb bone location
- Spawns a physic body, representing dismembered limb upon dismemberment

Weapon tracing
- Weapon traces are controlled by gameplay tags and animation events.
- Each attack section supplies active trace tags
- Tags resolve the relevant weapon sockets
- Socket positions are cached frame-to-frame for continuous delta sweeps (backtracing)
- Sphere sweeps detect targets during the active hit window
- Hit deduplication per attack window via TSet container
  
AI
<br>The enemy uses a minimal combat behavior loop:
<br>Acquire target → approach → activate attack ability
→ wait for ability completion → recovery delay → repeat
<br>The AI is deliberately limited: this project is a combat-systems study, not an AI showcase.

## Code Tour

| Area | Key implementation | What it demonstrates |
|---|---|---|
| Melee combo ability | [SL_PrimaryAttackAbility.cpp](Source/TPAction/Private/AbilitySystem/Abilities/SL_PrimaryAttackAbility.cpp) | GAS-driven combo flow, montage sections, gameplay events, tracing windows, and hit application |
| Weapon tracing | [SL_WeaponComponent.cpp](Source/TPAction/Private/Components/SL_WeaponComponent.cpp) | Tag-driven trace socket selection, frame-to-frame sphere sweeps, and Gameplay Ability target data |
| Limb system | [SL_LimbsComponent.cpp](Source/TPAction/Private/Components/SL_LimbsComponent.cpp) | Bone-to-limb mapping, limb health, armor, multipliers, dismemberment, and visual feedback |
| Damage calculation | [SL_ExecCalc_Damage.cpp](Source/TPAction/Private/AbilitySystem/ExecCalculations/SL_ExecCalc_Damage.cpp) | GAS execution calculation connecting damage, armor, and limb modifiers |
| Lock-on targeting (As part of the Character) | [SL_PlayerCharacter.cpp](Source/TPAction/Private/Characters/SL_PlayerCharacter.cpp) | Target selection, camera lock-on, target validation, and limb selection |
| Enemy attack behavior | [SL_BTTask_BaseAttack.cpp](Source/TPAction/Private/AI/Tasks/SL_BTTask_BaseAttack.cpp) | Latent Behavior Tree task, ability lifecycle handling, recovery delay, and abort cleanup |
| Gameplay tags | [GameplayTags.h](Source/TPAction/Public/Tags/GameplayTags.h) | Shared vocabulary used to connect abilities, animation events, tracing, and combat state |

## Tech Stack

- Unreal Engine 5
- C++
- Gameplay Ability System
- Gameplay Tags
- Behavior Trees
- Motion Warping
- UMG
- Enhanced Input

## Known Limitations

- Single-player prototype
- One enemy archetype and limited attack set
- Damage values are currently prototype-tuned
- No production multiplayer validation
- Limb effects are designed for demonstration rather than a complete equipment/dismemberment system
  
## Credits

All gameplay code and systems were implemented by me. 



