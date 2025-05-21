include config.mk

# Source directories
DIRS = core wm ui a11y util

# Core source files
CORE_SRC = core/dina.c core/config.c core/event.c
# Window management source files
WM_SRC = wm/window.c wm/monitor.c wm/tag.c wm/rules.c
# UI source files
UI_SRC = ui/drw.c
# Accessibility source files
A11Y_SRC = a11y/notify.c a11y/workspace_memory.c
# Utility source files
UTIL_SRC = util/util.c

# All source files
SRC = $(CORE_SRC) $(WM_SRC) $(UI_SRC) $(A11Y_SRC) $(UTIL_SRC)
OBJ = ${SRC:.c=.o}

# Header directories for include paths
INCS = -I. $(patsubst %,-I%,$(DIRS))

all: DINA

DINA: ensure_dirs ${OBJ}
	@echo "Linking DINA..."
	@${CC} -o $@ ${OBJ} ${LDFLAGS} || (echo "Failed to link DINA" && exit 1)
	@echo "Build successful!"

ensure_dirs:
	@echo "Checking module directories..."
	@for dir in ${DIRS}; do \
		if [ ! -d $$dir ]; then \
			echo "Creating directory: $$dir"; \
			mkdir -p $$dir; \
		fi; \
	done

clean:
	@echo "Cleaning build files..."
	@rm -f DINA ${OBJ} *.core
	@find . -name "*.o" -type f -delete
	@echo "Clean complete."

install: all
	@echo "Installing DINA system-wide..."
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f DINA ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/DINA
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@cp -f DINA.1 ${DESTDIR}${MANPREFIX}/man1/DINA.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/DINA.1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man5
	@cp -f dina-workspace.5 ${DESTDIR}${MANPREFIX}/man5/dina-workspace.5
	@chmod 644 ${DESTDIR}${MANPREFIX}/man5/dina-workspace.5
	@echo "DINA installed to ${DESTDIR}${PREFIX}/bin/DINA"

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
	rm -f ${MANPREFIX}/man5/dina-workspace.5
	rm -f ${HOME}/.local/bin/toggle-blackout ${HOME}/.local/bin/battery-watch \
	      ${HOME}/.local/bin/app-launcher ${HOME}/.local/bin/session-menu \
	      ${HOME}/.local/bin/start-orca ${HOME}/.local/bin/interactive-userinstall
	rm -f ${HOME}/.config/sxhkd/sxhkdrc

# Pattern rule for object files
%.o: %.c
	@echo "Compiling $<..."
	@${CC} -c ${ALL_CFLAGS} $< -o $@ || (echo "Failed to compile $<" && exit 1)

.PHONY: all clean install userinstall interactive-userinstall uninstall ensure_dirs