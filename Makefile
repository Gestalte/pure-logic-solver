SRC = .\resources
DEST = .\build\resources

all: make-build-folder copy-folder program run clean

make-build-folder:
	cmd /c if exist "build" rmdir /s /q "build"
	mkdir build

copy-folder:
	xcopy /s/i/D/y $(SRC) $(DEST)

program: PureLogicSolver.c
	gcc -o .\build\PureLogicSolver.exe PureLogicSolver.c -lraylib -lgdi32 -lwinmm -std=c99 -g -mwindows

run:
	.\build\PureLogicSolver.exe

clean:
	rmdir /s /q "build"