# Source files
SOURCES = src/collision.c src/init.c src/main.c src/process.c src/render.c src/resources.c

# Objects and deps are named after their source
OBJS = ${SOURCES:%.c=%.o}
DEPS = ${SOURCES:%.c=%.d}

# Name of executable
EXEC = allie

# Compiler and linker flags
# -MMD and -MP for generating .d-files
CFLAGS = -c -Wall -MMD -MP -std=c99 `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# Non-file based targets
.PHONY: clean rebuild

# Targets
all: ${EXEC}

${EXEC}: ${OBJS}
	${CC} -o $@ ${OBJS} ${LIBS}

# Include dependency files
-include ${DEPS}

# Make object file (compile .c into .o)
%.o: %.c
	${CC} ${CFLAGS} -o $@ $<

clean:
	-rm ${EXEC} ${OBJS}

rebuild: clean all

# EOF
