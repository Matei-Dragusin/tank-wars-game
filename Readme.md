# Tank Wars

A 2D artillery game where two tanks battle across a destructible terrain.

## Game Overview

Tank Wars is a turn-based artillery game where two players control tanks positioned on opposite sides of a procedurally generated terrain. Players must adjust their tank's position, turret angle, and firing power to hit the opponent's tank while adapting to the changing landscape that deforms after each shot.

## Features

- **Dynamic Terrain**: Procedurally generated terrain that deforms when hit by projectiles
- **Physics-Based Gameplay**: Projectiles follow realistic ballistic trajectories affected by gravity
- **Tank Controls**: Move tanks across the terrain, adjust turret angle and firing power
- **Realistic Movement**: Tanks move slower uphill and faster downhill
- **Visual Feedback**: Camera shake on explosions, projected trajectory visualization
- **Day/Night Cycle**: Gradually changing sky colors simulating time of day
- **Landslide Mechanics**: Steep terrain gradually erodes over time

## Controls

### Tank 1 (Red)
- **A/D**: Move left/right
- **W/S**: Rotate turret up/down
- **Q/E**: Increase/decrease firing power
- **Space**: Fire

### Tank 2 (Blue)
- **Left/Right Arrows**: Move left/right
- **Up/Down Arrows**: Rotate turret up/down
- **O/P**: Increase/decrease firing power
- **Enter**: Fire

## Technical Details

### Game Components

1. **Terrain System**
   - Procedural terrain generation using sine wave functions
   - Dynamic deformation at impact points
   - Gradual landslide effect for steep slopes
   - Height-based coloring for visual depth

2. **Tank Physics**
   - Tank rotation based on terrain slope
   - Speed adjustments based on slope angle
   - Realistic movement constraints on steep terrain

3. **Projectile System**
   - Ballistic trajectory calculation
   - Gravity-affected movement
   - Collision detection with terrain and tanks

4. **Visual Effects**
   - Camera shake on explosions
   - Trajectory prediction line
   - Day/night cycle with changing sky colors
   - Health bars for each tank

### Implementation Details

- Built with OpenGL for rendering
- Uses vertex-based meshes for all game elements
- Custom physics implementation for projectiles and terrain interaction

## Game Flow

1. Players take turns firing at each other
2. Each shot deforms the terrain, changing the battlefield
3. Tanks must adapt to the changing landscape
4. Game continues until one tank's health reaches zero

## Technical Architecture

The game is structured around several key classes:

- **Tema1**: Main game class managing the game loop, rendering, and input handling
- **Tank**: Handles tank movement, turret rotation, and firing logic
- **Projectile**: Manages projectile movement and collisions
- **Terrain**: Handles terrain generation, deformation, and landslide effects

## Advanced Mechanics

- **Terrain Deformation**: Each projectile impact creates a crater in the terrain
- **Landslide Effect**: Steep terrain gradually erodes, adding strategic depth
- **Adaptive Tank Movement**: Tank speed and ability to climb slopes depends on the terrain angle
- **Trajectory Prediction**: Players can see the projected path of their shot before firing
- **Realistic Damage**: Damage is applied based on direct hits to enemy tanks

## Development Challenges

- Implementing smooth terrain deformation
- Ensuring realistic tank movement on varying slopes
- Calculating accurate projectile trajectories
- Creating visual feedback for player actions
- Balancing gameplay mechanics for fair competition

## Future Improvements

- Power-ups and special ammunition types
- More terrain types and weather effects
- Multiplayer networking capability
- AI opponents with varying difficulty levels
- Additional visual effects and sound design