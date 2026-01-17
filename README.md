# pure-logic-solver

This is a program that mimics the game [Pure Logic](https://store.steampowered.com/app/1861500/Pure_Logic/) but that lets you set up custom levels and gives immeditate feedback instead of only validating correctness when all lines have been set.

I made it because I became frustrated with the later levels which feature many gates and few starting lines. This leads to trial and error gameplay and, when you make a mistake, having to remember the whole state of what you had, because the level resets to it's starting state.

This does not copy any of the game's levels, it only functions as a quality of life companion to the game.

## Screen shots

### Set how many gates there should be

![layout](https://github.com/Gestalte/pure-logic-solver/screenshots/blob/master/layout.PNG?raw=true)

### Set which lines should not be editable and what type of gate should be used

![setup](https://github.com/Gestalte/pure-logic-solver/screenshots/blob/master/setup.PNG?raw=true)

### Play the game and get immeditate feedback

![unvalidated](https://github.com/Gestalte/pure-logic-solver/screenshots/blob/master/unvalidated.PNG?raw=true)

### Level completed

![validation](https://github.com/Gestalte/pure-logic-solver/screenshots/blob/master/validation.PNG?raw=true)

## How to build

The program is written for Windows and assumes Windows filepaths.

### Batch file

- Requires CL and Raylib
- You need to find and run `vcvarsall.bat` for CL to work while using CMD/Powershell.
- Edit `Build.bat` and set `RAYLIB_SRC` to where you have the Raylib/src folder is located.
- run `Build.bat` using CMD/Powershell. A build folder is created with the .exe file.

### Make

- Requires Raylib in your PATH
- Requires GNU Make
- Run `make` or `make run` in the root folder using CMD/Powershell. Running only `make` will delete the .exe after the program is closed.
