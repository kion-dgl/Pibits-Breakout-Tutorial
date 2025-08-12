# Brickout Game Development TODO

## Project Requirements
- **Language**: C
- **Graphics**: SDL2 
- **Target Platform**: Raspberry Pi with RetroPie
- **Resolution**: 640x480 window
- **Audio**: SDL2_mixer
- **Assets**: Located in `docs/assets/` and `docs/img/`

## Phase 1: Core Setup
- [ ] **Project Structure Setup**
  - [ ] Create src/ directory structure
  - [ ] Create include/ directory for headers
  - [ ] Set up Makefile for SDL2 compilation
  - [ ] Create basic main.c with SDL2 initialization
  - [ ] Test SDL2 window creation at 640x480

- [ ] **Asset Loading System**
  - [ ] Implement texture loading with SDL2_image
  - [ ] Create asset manager for textures
  - [ ] Load background.png from docs/img/
  - [ ] Load UI elements from docs/assets/UI/
  - [ ] Test texture rendering

- [ ] **Audio System**
  - [ ] Initialize SDL2_mixer
  - [ ] Load BGM files from docs/assets/BGM/
  - [ ] Load SFX files from docs/assets/SFX/
  - [ ] Create audio playback functions

## Phase 2: Game States
- [ ] **State Management System**
  - [ ] Create game state enum (TITLE, GAMEPLAY, GAMEOVER, COMPLETE)
  - [ ] Implement state switching mechanism
  - [ ] Create state-specific update/render functions

- [ ] **Title Screen (index.html mockup)**
  - [ ] Render background with docs/img/background.png
  - [ ] Display brickout logo from docs/img/brickout-logo.webp
  - [ ] Show Dashie character from docs/img/dashie.webp
  - [ ] Implement menu with "Start Game" and "Quit Game"
  - [ ] Add spinning arrow selector from docs/assets/UI/arrow_*.png
  - [ ] Handle keyboard input (Arrow keys, Enter, A button)
  - [ ] Play Title Screen.wav BGM

## Phase 3: Core Gameplay
- [ ] **Game Objects**
  - [ ] Create Ball struct with position, velocity, size
  - [ ] Create Paddle struct with position, size
  - [ ] Create Brick struct with position, color, destroyed state
  - [ ] Load ball texture from docs/assets/UI/ballBlue.png
  - [ ] Load paddle texture from docs/assets/UI/paddleBlu.png
  - [ ] Load brick textures from docs/assets/UI/element_*_rectangle.png

- [ ] **Ball Physics**
  - [ ] Implement ball movement and velocity
  - [ ] Ball-wall collision detection and bounce
  - [ ] Ball-paddle collision detection and bounce
  - [ ] Ball-brick collision detection and destruction
  - [ ] Play appropriate SFX for each collision type

- [ ] **Paddle Control**
  - [ ] Keyboard input for paddle movement (left/right arrows)
  - [ ] Paddle boundary constraints
  - [ ] Smooth paddle movement

- [ ] **Brick System**
  - [ ] Create brick grid for each stage
  - [ ] Implement brick destruction
  - [ ] Particle effects when bricks break (docs/assets/UI/particle*.png)
  - [ ] Track remaining bricks for level completion

## Phase 4: Game Progression
- [ ] **Stage System**
  - [ ] Implement 5 different stages matching gameplay.html mockups
  - [ ] Stage 1: Classic horizontal rows
  - [ ] Stage 2: Checkerboard pattern  
  - [ ] Stage 3: Diamond formation with gaps
  - [ ] Stage 4: Fortress pattern (6 rows)
  - [ ] Stage 5: Maze pattern (7 rows)
  - [ ] Stage-specific BGM (Stage_1.wav through Stage_5.wav)

- [ ] **Lives and Scoring System**
  - [ ] Implement 3 lives system
  - [ ] Score calculation for brick destruction
  - [ ] Display lives using heart icons
  - [ ] Display score on screen
  - [ ] Game over when lives reach 0

- [ ] **UI and HUD**
  - [ ] Lives display in top-left (ball icons from docs/assets/UI/)
  - [ ] Score display in top-right
  - [ ] Stage indicator
  - [ ] Pause functionality

## Phase 5: End Game States
- [ ] **Game Over Screen (gameover.html mockup)**
  - [ ] Show Kion defeated character (docs/img/kion-ded.webp)
  - [ ] Display "Continue?" text
  - [ ] "Yes" and "No" menu options
  - [ ] Muted background filter effect
  - [ ] Play GameOver.wav BGM

- [ ] **Game Complete Screen (complete.html mockup)**
  - [ ] Show Kion happy character (docs/img/kion-happi.webp)
  - [ ] Display "Complete!" text
  - [ ] "Play Again" and "Quit Game" options
  - [ ] Bright, celebratory background
  - [ ] Play GameComplete.wav BGM

## Phase 6: Polish and Optimization
- [ ] **Visual Effects**
  - [ ] Implement dithering effects for atmosphere
  - [ ] Add brick shine animations
  - [ ] Smooth state transitions
  - [ ] Screen shake effects for impacts

- [ ] **Audio Polish**
  - [ ] Background music looping
  - [ ] Sound effect volume balancing
  - [ ] Menu navigation sounds (menu_select.mp3)
  - [ ] Life lost sound (lose_life.mp3)

- [ ] **Performance**
  - [ ] Optimize for Raspberry Pi performance
  - [ ] Efficient collision detection
  - [ ] Memory management
  - [ ] 60 FPS target maintenance

## Phase 7: Platform Integration
- [ ] **RetroPie Compatibility**
  - [ ] Test on Raspberry Pi hardware
  - [ ] Controller input mapping (Xbox controller support)
  - [ ] Resolution scaling if needed
  - [ ] Performance optimization for ARM processor

- [ ] **Build System**
  - [ ] Cross-compilation setup for Pi
  - [ ] Asset packaging
  - [ ] Installation scripts
  - [ ] Documentation for building and running

## File Structure
```
Pibits-Breakout-Tutorial/
├── src/
│   ├── main.c
│   ├── game_state.c
│   ├── title_screen.c
│   ├── gameplay.c
│   ├── ball.c
│   ├── paddle.c
│   ├── brick.c
│   ├── audio.c
│   └── texture_manager.c
├── include/
│   ├── game.h
│   ├── game_state.h
│   ├── ball.h
│   ├── paddle.h
│   ├── brick.h
│   ├── audio.h
│   └── texture_manager.h
├── docs/assets/ (existing)
├── docs/img/ (existing)
├── Makefile
└── README.md
```

## Dependencies
- SDL2
- SDL2_image (for PNG/WEBP loading)
- SDL2_mixer (for audio)
- SDL2_ttf (for text rendering)

## Build Command Example
```bash
gcc -o brickout src/*.c -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm
```