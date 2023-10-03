@echo off

del plot.html
del data.txt

gcc -o main src/main.c -Iinclude/Python311 -Iinclude/SDL_ttf -Iinclude/SDL2 -Llib/Python311 -Llib/SDL_ttf -Llib/SDL2 -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lpython311 -lpthread

main.exe

python src/Python.py

start "" "file:///%cd%/plot.html"



