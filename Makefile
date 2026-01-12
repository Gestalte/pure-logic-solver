all: program run clean

program: PureLogicSolver.c
	gcc -o PureLogicSolver.exe PureLogicSolver.c -lraylib -lgdi32 -lwinmm -std=c99 -g -mwindows

run:
	PureLogicSolver.exe

clean:
	erase PureLogicSolver.exe
