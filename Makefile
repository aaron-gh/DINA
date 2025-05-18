include config.mk

SRC = dwm.c drw.c util.c
OBJ = ${SRC:.c=.o}

all: dwm

dwm: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS} -lX11 -lXinerama

clean:
	rm -f dwm ${OBJ} *.core

install: all
	# Install dwm binary system-wide
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f dwm ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/dwm

userinstall:
	# User-local install for scripts and configs
	mkdir -p ${HOME}/.local/bin
	cp -f scripts/* ${HOME}/.local/bin
	chmod +x ${HOME}/.local/bin/*

	mkdir -p ${HOME}/.config/sxhkd
	cp -f sxhkd/sxhkdrc ${HOME}/.config/sxhkd/sxhkdrc

uninstall:
	rm -f ${PREFIX}/bin/dwm
	rm -f ${HOME}/.local/bin/toggle-blackout ${HOME}/.local/bin/battery-watch \
	      ${HOME}/.local/bin/app-launcher ${HOME}/.local/bin/session-menu
	rm -f ${HOME}/.config/sxhkd/sxhkdrc

.PHONY: all clean install userinstall uninstall
