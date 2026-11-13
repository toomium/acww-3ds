# AI disclosure

This project is developed with help from Claude.

## What the AI does

- Writes much of the C code for the 3DS backend and the Python tools.
- Researches and documents the decompilation and the DS and 3DS hardware.
- Reviews build failures and measurements, and proposes fixes.

## How AI-written changes are handled

- They arrive as pull requests, never as direct pushes to `main`.
- Commits written with AI help carry a `Co-Authored-By: Claude` line, so the
  history shows where it was involved.
- Every change is built by CI and tested by a human (in Azahar, and on a real
  Old 3DS and New 3DS for performance-sensitive work) before it is merged.
- The project owner decides what gets merged and is responsible for the
  repository's contents.

## What the AI is not used for

- Generating, recreating or "cleaning up" game assets. All game content comes
  from the player's own cartridge dump.
- Reproducing code from leaked sources or from ports that bundle game assets.
