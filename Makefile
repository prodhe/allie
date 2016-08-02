# Vars
EXEC = allie
SRC_FILES = $(shell find . -name *.c)
OBJ_FILES = ${SRC_FILES:%.c=%.o}

CFLAGS = -Wall -std=c99 `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# Targets
all: ${EXEC}

${EXEC}: ${OBJ_FILES}
	gcc -o ${EXEC} ${OBJ_FILES} ${LIBS}

${OBJ_FILES}: ${SRC_FILES}
	echo ${SRC_FILES}
#gcc -c $< -o $@ ${CFLAGS}


.PHONY: clean rebuild
clean:
	-rm ${EXEC} ${OBJ_FILES}

rebuild: clean all


# build:
# 	gcc -c src/*.c -std=c99 `sdl2-config --cflags`
# 	gcc -o allie-run *.o `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer
# 	rm *.o
# 
# clean:
# 	rm allie src/*.o
