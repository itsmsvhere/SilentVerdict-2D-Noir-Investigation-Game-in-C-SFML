# Silent Verdict 🕵️

A 2D noir investigation thriller built in C++ and SFML.
Question suspects, analyse evidence, and deliver the verdict.

## Overview

Silent Verdict is a fully UI-driven investigation game where the player
interviews four suspects in a murder case set in a remote forest.
Every response updates a hidden suspicion score. Only the correct
arrest reveals the full motive behind the crime.

Built entirely from scratch using C++17 and SFML — no game engine,
no templates, no shortcuts.

## Tech Stack

- **Language:** C++17
- **Library:** SFML 2.6 (Graphics, Window, System, Audio)
- **Compiler:** GCC 13.2 / MinGW-w64
- **Architecture:** Custom OOP engine with modular scene management

## Architecture

| Module | Responsibility |
|---|---|
| `Game` | Core game loop, scene rendering, input handling |
| `SceneManager` | Scene state machine (Menu → Intro → Forest → Interrogation → Verdict) |
| `UIManager` | Button system, font loading, panel/text helpers |
| `DialogueSystem` | Suspect responses, mocked suspicion scoring logic |
| `EvidenceSystem` | Evidence data model |
| `AudioManager` | Sound loading and playback with mute toggle |

## Features

- 5 fully rendered scenes with smooth black fade transitions
- Clickable evidence icons with animated reveal popups
- 4 suspects with unique dialogue, colour coding, and emotion states
- Suspect portraits shift from Composed → Distressed → Arrested
- Suspicion scoring system drives the final verdict
- Full motive backstory revealed on correct arrest
- Background image support with graceful fallback rendering
- Click, transition, verdict, and character sounds
- Mute toggle always accessible

## Build

```bash
g++ -std=c++17 main.cpp Game.cpp SceneManager.cpp UIManager.cpp \
    DialogueSystem.cpp EvidenceSystem.cpp AudioManager.cpp \
    -o SilentVerdict.exe \
    -IC:\SFML\include -LC:\SFML\lib \
    -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

Copy DLLs next to the executable:
```bash
copy C:\SFML\bin\sfml-graphics-2.dll .
copy C:\SFML\bin\sfml-window-2.dll .
copy C:\SFML\bin\sfml-system-2.dll .
copy C:\SFML\bin\sfml-audio-2.dll .
copy C:\SFML\bin\openal32.dll .
```

## Gameplay

| Scene | Action |
|---|---|
| Main Menu | Click Start Game |
| Intro | Press ENTER |
| Forest | Click evidence icons → Press ENTER |
| Interrogation | Ask all 5 questions per suspect → ENTER to advance |
| Verdict | Click an arrest button |
| Result | Press ENTER to restart |

## Assets

Place background images in `assets/` and character/evidence images
in `assets/images/`. The game runs fully without them using built-in
SFML shape rendering as fallback.

## Author

**M S Vishaal**
B.Tech — Artificial Intelligence & Data Science
Saveetha School of Engineering, SIMATS
