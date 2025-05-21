include config.mk

SRC = dwm.c drw.c util.c
OBJ = ${SRC:.c=.o}

all: DINA

DINA: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS} -lX11 -lXinerama

clean:
	rm -f DINA ${OBJ} *.core

install: all
	# Install DINA binary system-wide
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f DINA ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/DINA
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	cp -f DINA.1 ${DESTDIR}${MANPREFIX}/man1/DINA.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/DINA.1

userinstall:
	# Run the interactive userinstall script
	@echo "Running interactive setup..."
	@if [ -x "${HOME}/.local/bin/interactive-userinstall" ]; then \
		${HOME}/.local/bin/interactive-userinstall; \
	else \
		./scripts/interactive-userinstall; \
	fi

# Keep this for backwards compatibility
interactive-userinstall: userinstall

uninstall:
	rm -f ${PREFIX}/bin/DINA
	rm -f ${MANPREFIX}/man1/DINA.1
	rm -f ${HOME}/.local/bin/toggle-blackout ${HOME}/.local/bin/battery-watch \
	      ${HOME}/.local/bin/app-launcher ${HOME}/.local/bin/session-menu \
	      ${HOME}/.local/bin/start-orca ${HOME}/.local/bin/interactive-userinstall
	rm -f ${HOME}/.config/sxhkd/sxhkdrc

.PHONY: all clean install userinstall interactive-userinstall uninstall
