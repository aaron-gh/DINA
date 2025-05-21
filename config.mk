# DINA version
VERSION = 1.1

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# Use pkg-config to get correct paths automatically if available
# Otherwise fall back to reasonable defaults for Linux

# Check if pkg-config is available
PKG_CONFIG := $(shell which pkg-config 2>/dev/null)

ifeq ($(PKG_CONFIG),)
    # Fallback if pkg-config is not available
    X11INC = /usr/include/X11
    X11LIB = /usr/lib
    FREETYPEINC = /usr/include/freetype2
    XINEINC = /usr/include/X11/extensions
    INCS = -I/usr/include -I$(X11INC) -I$(FREETYPEINC) -I$(XINEINC)
    LIBS = -L/usr/lib -L$(X11LIB) -lX11 -lXinerama -lXft -lfontconfig -lfreetype
else
    # Use pkg-config to get cflags and libs automatically
    X11_CFLAGS := $(shell $(PKG_CONFIG) --cflags x11)
    X11_LIBS := $(shell $(PKG_CONFIG) --libs x11)
    
    XINERAMA_CFLAGS := $(shell $(PKG_CONFIG) --cflags xinerama)
    XINERAMA_LIBS := $(shell $(PKG_CONFIG) --libs xinerama)
    
    XFT_CFLAGS := $(shell $(PKG_CONFIG) --cflags xft fontconfig)
    XFT_LIBS := $(shell $(PKG_CONFIG) --libs xft fontconfig)
    
    FREETYPE_CFLAGS := $(shell $(PKG_CONFIG) --cflags freetype2)
    
    INCS = $(X11_CFLAGS) $(XINERAMA_CFLAGS) $(XFT_CFLAGS) $(FREETYPE_CFLAGS)
    LIBS = $(X11_LIBS) $(XINERAMA_LIBS) $(XFT_LIBS)
endif

# Xinerama, comment if you don't want it
XINERAMAFLAGS = -DXINERAMA

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700L -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS}
# Main flags - use -g for debug builds
CFLAGS   = -std=c99 -pedantic -Wall -Wextra -Wno-deprecated-declarations -Wno-unused-parameter -Os 
# Add module directories to include path
MODDIR_INCS = -I. -Icore -Iwm -Iui -Ia11y -Iutil

# Combined flags
# Explicitly add freetype2 headers for Xft
ALL_CFLAGS = $(CFLAGS) $(INCS) $(MODDIR_INCS) $(CPPFLAGS) -I/usr/include/freetype2
LDFLAGS  = $(LIBS)

# compiler and linker
CC = cc