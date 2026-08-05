# Targeted Limb Combat Prototype

A UE5 C++ technical gameplay study exploring targeted limb combat in a third-person melee game.

Inspired by the design problem of readable limb targeting in action games.
This is an original prototype, not a recreation of any existing game.

[Gameplay video](...) · [Portfolio](...)

![Gameplay GIF: lock-on, limb selection, hit, dismemberment](...)

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

flowchart LR
    A["Player input"] --> B["GAS attack ability"]
    B --> C["Animation montage / notify"]
    C --> D["Weapon socket trace"]
    D --> E["Hit result + bone"]
    E --> F["Limb mapping"]
    F --> G["Damage calculation: armor + multiplier"]
    G --> H["Limb health / destruction"]
    H --> I["HUD and visual feedback"]

  ## Technical Highlights

Limb damage
Each limb stores:
- Gameplay Tag
- Skeletal bone mapping
- Health and armor
- Damage multiplier
- Supporting-limb flag
- Destruction feedback assets
The damage execution uses the physical hit bone to resolve the affected limb before applying armor and damage multipliers.

Weapon tracing
- Weapon traces are controlled by gameplay tags and animation events.
- Each attack section supplies active trace tags
- Tags resolve the relevant weapon sockets
- Socket positions are cached between frames
- Sphere sweeps detect targets during the active hit window
- Each actor can only be hit once per trace window
  
AI
The enemy uses a minimal combat behavior loop:
Acquire target → approach → activate attack ability
→ wait for ability completion → recovery delay → repeat
The AI is deliberately limited: this project is a combat-systems study, not an AI showcase.

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



