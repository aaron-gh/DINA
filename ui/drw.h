/**
 * @file drw.h
 * @brief Drawing functions for DINA
 * 
 * This file contains drawing utilities for rendering UI elements.
 * Original code from dwm with added documentation.
 */

#ifndef _DRW_H
#define _DRW_H

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

/**
 * @brief Cursor structure
 */
typedef struct {
	Cursor cursor;
} Cur;

/**
 * @brief Font structure
 */
typedef struct Fnt {
	Display *dpy;
	unsigned int h;
	XftFont *xfont;
	FcPattern *pattern;
	struct Fnt *next;
} Fnt;

/**
 * @brief Color scheme indices
 */
enum { ColFg, ColBg, ColBorder }; /* Clr scheme index */

/**
 * @brief Color type (uses Xft colors)
 */
typedef XftColor Clr;

/**
 * @brief Drawing context
 */
typedef struct {
	unsigned int w, h;
	Display *dpy;
	int screen;
	Window root;
	Drawable drawable;
	GC gc;
	Clr *scheme;
	Fnt *fonts;
} Drw;

/* Drawable abstraction */

/**
 * @brief Create a new drawing context
 * 
 * @param dpy X display
 * @param screen Screen number
 * @param win Window
 * @param w Width
 * @param h Height
 * @return Drawing context
 */
Drw *drw_create(Display *dpy, int screen, Window win, unsigned int w, unsigned int h);

/**
 * @brief Resize a drawing context
 * 
 * @param drw Drawing context
 * @param w New width
 * @param h New height
 */
void drw_resize(Drw *drw, unsigned int w, unsigned int h);

/**
 * @brief Free a drawing context
 * 
 * @param drw Drawing context to free
 */
void drw_free(Drw *drw);

/* Fnt abstraction */

/**
 * @brief Create a font set
 * 
 * @param drw Drawing context
 * @param fonts Array of font names
 * @param fontcount Number of fonts
 * @return Font set
 */
Fnt *drw_fontset_create(Drw* drw, const char *fonts[], size_t fontcount);

/**
 * @brief Free a font set
 * 
 * @param set Font set to free
 */
void drw_fontset_free(Fnt* set);

/**
 * @brief Get width of text with current font set
 * 
 * @param drw Drawing context
 * @param text Text to measure
 * @return Width in pixels
 */
unsigned int drw_fontset_getwidth(Drw *drw, const char *text);

/**
 * @brief Get width of text with current font set (with length limit)
 * 
 * @param drw Drawing context
 * @param text Text to measure
 * @param n Maximum number of characters
 * @return Width in pixels
 */
unsigned int drw_fontset_getwidth_clamp(Drw *drw, const char *text, unsigned int n);

/**
 * @brief Get text extents for a font
 * 
 * @param font Font to use
 * @param text Text to measure
 * @param len Length of text
 * @param w Width result
 * @param h Height result
 */
void drw_font_getexts(Fnt *font, const char *text, unsigned int len, unsigned int *w, unsigned int *h);

/* Colorscheme abstraction */

/**
 * @brief Create a color
 * 
 * @param drw Drawing context
 * @param dest Destination color
 * @param clrname Color name
 */
void drw_clr_create(Drw *drw, Clr *dest, const char *clrname);

/**
 * @brief Create a color scheme
 * 
 * @param drw Drawing context
 * @param clrnames Array of color names
 * @param clrcount Number of colors
 * @return Color scheme
 */
Clr *drw_scm_create(Drw *drw, const char *clrnames[], size_t clrcount);

/* Cursor abstraction */

/**
 * @brief Create a cursor
 * 
 * @param drw Drawing context
 * @param shape Cursor shape
 * @return Cursor
 */
Cur *drw_cur_create(Drw *drw, int shape);

/**
 * @brief Free a cursor
 * 
 * @param drw Drawing context
 * @param cursor Cursor to free
 */
void drw_cur_free(Drw *drw, Cur *cursor);

/* Drawing context manipulation */

/**
 * @brief Set the font set for a drawing context
 * 
 * @param drw Drawing context
 * @param set Font set
 */
void drw_setfontset(Drw *drw, Fnt *set);

/**
 * @brief Set the color scheme for a drawing context
 * 
 * @param drw Drawing context
 * @param scm Color scheme
 */
void drw_setscheme(Drw *drw, Clr *scm);

/* Drawing functions */

/**
 * @brief Draw a rectangle
 * 
 * @param drw Drawing context
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h Height
 * @param filled Whether to fill the rectangle
 * @param invert Whether to invert the colors
 */
void drw_rect(Drw *drw, int x, int y, unsigned int w, unsigned int h, int filled, int invert);

/**
 * @brief Draw text
 * 
 * @param drw Drawing context
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h Height
 * @param lpad Left padding
 * @param text Text to draw
 * @param invert Whether to invert the colors
 * @return Width of drawn text
 */
int drw_text(Drw *drw, int x, int y, unsigned int w, unsigned int h, unsigned int lpad, const char *text, int invert);

/* Map functions */

/**
 * @brief Map drawing to a window
 * 
 * @param drw Drawing context
 * @param win Window
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h Height
 */
void drw_map(Drw *drw, Window win, int x, int y, unsigned int w, unsigned int h);

#endif /* _DRW_H */