SRC = dbs.c
OBJ = ${SRC:.c=.o}
CONFIG = config.h
X11INC = /usr/local/include
X11LIB = /usr/local/lib
CFLAGS += -I${X11INC} -L${X11LIB}
LDFLAGS += -lX11

all: dbs
	@echo "CFLAGS = ${CFLAGS}"
	@echo "LDFLAGS = ${LDFLAGS}"

${CONFIG}:
	@touch $@

${OBJ}: ${CONFIG}

dbs: ${OBJ}
	${CC} ${CFLAGS} -o $@ ${OBJ} ${LDFLAGS}

.PHONY: clean

clean:
	@rm -f *.o
	@rm -f dbs
