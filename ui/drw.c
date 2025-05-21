/**
 * @file drw.c
 * @brief Drawing functions for DINA
 * 
 * This file contains drawing utilities for rendering UI elements.
 * Original code from dwm with added documentation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include "drw.h"
#include "util/util.h"

#define UTF_INVALID 0xFFFD

/**
 * @brief Decode a UTF-8 character
 * 
 * @param s_in UTF-8 string
 * @param u Pointer to store codepoint
 * @param err Pointer to store error flag
 * @return Number of bytes consumed
 */
static int
utf8decode(const char *s_in, long *u, int *err)
{
	static const unsigned char lens[] = {
		/* 0XXXX */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		/* 10XXX */ 0, 0, 0, 0, 0, 0, 0, 0,  /* invalid */
		/* 110XX */ 2, 2, 2, 2,
		/* 1110X */ 3, 3,
		/* 11110 */ 4,
		/* 11111 */ 0,  /* invalid */
	};
	static const unsigned char leading_mask[] = { 0x7F, 0x1F, 0x0F, 0x07 };
	static const unsigned int overlong[] = { 0x0, 0x80, 0x0800, 0x10000 };

	const unsigned char *s = (const unsigned char *)s_in;
	int len = lens[*s >> 3];
	*u = UTF_INVALID;
	*err = 1;
	if (len == 0)
		return 1;

	long cp = s[0] & leading_mask[len - 1];
	for (int i = 1; i < len; ++i) {
		if (s[i] == '\0' || (s[i] & 0xC0) != 0x80)
			return i;
		cp = (cp << 6) | (s[i] & 0x3F);
	}
	/* out of range, surrogate, overlong encoding */
	if (cp > 0x10FFFF || (cp >> 11) == 0x1B || cp < overlong[len - 1])
		return len;

	*err = 0;
	*u = cp;
	return len;
}

/**
 * @brief Create a new drawing context
 * 
 * @param dpy X display
 * @param screen Screen number
 * @param root Window
 * @param w Width
 * @param h Height
 * @return Drawing context
 */
Drw *
drw_create(Display *dpy, int screen, Window root, unsigned int w, unsigned int h)
{
	Drw *drw = ecalloc(1, sizeof(Drw));

	drw->dpy = dpy;
	drw->screen = screen;
	drw->root = root;
	drw->w = w;
	drw->h = h;
	drw->drawable = XCreatePixmap(dpy, root, w, h, DefaultDepth(dpy, screen));
	drw->gc = XCreateGC(dpy, root, 0, NULL);
	XSetLineAttributes(dpy, drw->gc, 1, LineSolid, CapButt, JoinMiter);

	return drw;
}

/**
 * @brief Resize a drawing context
 * 
 * @param drw Drawing context
 * @param w New width
 * @param h New height
 */
void
drw_resize(Drw *drw, unsigned int w, unsigned int h)
{
	if (!drw)
		return;

	drw->w = w;
	drw->h = h;
	if (drw->drawable)
		XFreePixmap(drw->dpy, drw->drawable);
	drw->drawable = XCreatePixmap(drw->dpy, drw->root, w, h, DefaultDepth(drw->dpy, drw->screen));
}

/**
 * @brief Free a drawing context
 * 
 * @param drw Drawing context to free
 */
void
drw_free(Drw *drw)
{
	XFreePixmap(drw->dpy, drw->drawable);
	XFreeGC(drw->dpy, drw->gc);
	drw_fontset_free(drw->fonts);
	free(drw);
}

/**
 * @brief Create a font from a name or pattern
 * 
 * This function is an implementation detail. Library users should use
 * drw_fontset_create instead.
 * 
 * @param drw Drawing context
 * @param fontname Font name
 * @param fontpattern Font pattern
 * @return Font structure
 */
static Fnt *
xfont_create(Drw *drw, const char *fontname, FcPattern *fontpattern)
{
	Fnt *font;
	XftFont *xfont = NULL;
	FcPattern *pattern = NULL;

	if (fontname) {
		/* Using the pattern found at font->xfont->pattern does not yield the
		 * same substitution results as using the pattern returned by
		 * FcNameParse; using the latter results in the desired fallback
		 * behaviour whereas the former just results in missing-character
		 * rectangles being drawn, at least with some fonts. */
		if (!(xfont = XftFontOpenName(drw->dpy, drw->screen, fontname))) {
			fprintf(stderr, "error, cannot load font from name: '%s'\n", fontname);
			return NULL;
		}
		if (!(pattern = FcNameParse((FcChar8 *) fontname))) {
			fprintf(stderr, "error, cannot parse font name to pattern: '%s'\n", fontname);
			XftFontClose(drw->dpy, xfont);
			return NULL;
		}
	} else if (fontpattern) {
		if (!(xfont = XftFontOpenPattern(drw->dpy, fontpattern))) {
			fprintf(stderr, "error, cannot load font from pattern.\n");
			return NULL;
		}
	} else {
		die("no font specified.");
	}

	font = ecalloc(1, sizeof(Fnt));
	font->xfont = xfont;
	font->pattern = pattern;
	font->h = xfont->ascent + xfont->descent;
	font->dpy = drw->dpy;

	return font;
}

/**
 * @brief Free a font
 * 
 * @param font Font to free
 */
static void
xfont_free(Fnt *font)
{
	if (!font)
		return;
	if (font->pattern)
		FcPatternDestroy(font->pattern);
	XftFontClose(font->dpy, font->xfont);
	free(font);
}

/**
 * @brief Create a font set
 * 
 * @param drw Drawing context
 * @param fonts Array of font names
 * @param fontcount Number of fonts
 * @return Font set
 */
Fnt*
drw_fontset_create(Drw* drw, const char *fonts[], size_t fontcount)
{
	Fnt *cur, *ret = NULL;
	size_t i;

	if (!drw || !fonts)
		return NULL;

	for (i = 1; i <= fontcount; i++) {
		if ((cur = xfont_create(drw, fonts[fontcount - i], NULL))) {
			cur->next = ret;
			ret = cur;
		}
	}
	return (drw->fonts = ret);
}

/**
 * @brief Free a font set
 * 
 * @param font Font set to free
 */
void
drw_fontset_free(Fnt *font)
{
	if (font) {
		drw_fontset_free(font->next);
		xfont_free(font);
	}
}

/**
 * @brief Create a color
 * 
 * @param drw Drawing context
 * @param dest Destination color
 * @param clrname Color name
 */
void
drw_clr_create(Drw *drw, Clr *dest, const char *clrname)
{
	if (!drw || !dest || !clrname)
		return;

	if (!XftColorAllocName(drw->dpy, DefaultVisual(drw->dpy, drw->screen),
	                       DefaultColormap(drw->dpy, drw->screen),
	                       clrname, dest))
		die("error, cannot allocate color '%s'", clrname);
}

/**
 * @brief Create a color scheme
 * 
 * @param drw Drawing context
 * @param clrnames Array of color names
 * @param clrcount Number of colors
 * @return Color scheme
 */
Clr *
drw_scm_create(Drw *drw, const char *clrnames[], size_t clrcount)
{
	size_t i;
	Clr *ret;

	/* need at least two colors for a scheme */
	if (!drw || !clrnames || clrcount < 2 || !(ret = ecalloc(clrcount, sizeof(XftColor))))
		return NULL;

	for (i = 0; i < clrcount; i++)
		drw_clr_create(drw, &ret[i], clrnames[i]);
	return ret;
}

/**
 * @brief Set the font set for a drawing context
 * 
 * @param drw Drawing context
 * @param set Font set
 */
void
drw_setfontset(Drw *drw, Fnt *set)
{
	if (drw)
		drw->fonts = set;
}

/**
 * @brief Set the color scheme for a drawing context
 * 
 * @param drw Drawing context
 * @param scm Color scheme
 */
void
drw_setscheme(Drw *drw, Clr *scm)
{
	if (drw)
		drw->scheme = scm;
}

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
void
drw_rect(Drw *drw, int x, int y, unsigned int w, unsigned int h, int filled, int invert)
{
	if (!drw || !drw->scheme)
		return;
	XSetForeground(drw->dpy, drw->gc, invert ? drw->scheme[ColBg].pixel : drw->scheme[ColFg].pixel);
	if (filled)
		XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w, h);
	else
		XDrawRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w - 1, h - 1);
}

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
int
drw_text(Drw *drw, int x, int y, unsigned int w, unsigned int h, unsigned int lpad, const char *text, int invert)
{
	int ty, ellipsis_x = 0;
	unsigned int tmpw, ew, ellipsis_w = 0, ellipsis_len, hash, h0, h1;
	XftDraw *d = NULL;
	Fnt *usedfont, *curfont, *nextfont;
	int utf8strlen, utf8charlen, utf8err, render = x || y || w || h;
	long utf8codepoint = 0;
	const char *utf8str;
	FcCharSet *fccharset;
	FcPattern *fcpattern;
	FcPattern *match;
	XftResult result;
	int charexists = 0, overflow = 0;
	/* keep track of a couple codepoints for which we have no match. */
	static unsigned int nomatches[128], ellipsis_width, invalid_width;
	static const char invalid[] = "�";

	if (!drw || (render && (!drw->scheme || !w)) || !text || !drw->fonts)
		return 0;

	if (!render) {
		w = invert ? invert : ~invert;
	} else {
		XSetForeground(drw->dpy, drw->gc, drw->scheme[invert ? ColFg : ColBg].pixel);
		XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w, h);
		if (w < lpad)
			return x + w;
		d = XftDrawCreate(drw->dpy, drw->drawable,
		                  DefaultVisual(drw->dpy, drw->screen),
		                  DefaultColormap(drw->dpy, drw->screen));
		x += lpad;
		w -= lpad;
	}

	usedfont = drw->fonts;
	if (!ellipsis_width && render)
		ellipsis_width = drw_fontset_getwidth(drw, "...");
	if (!invalid_width && render)
		invalid_width = drw_fontset_getwidth(drw, invalid);
	while (1) {
		ew = ellipsis_len = utf8err = utf8charlen = utf8strlen = 0;
		utf8str = text;
		nextfont = NULL;
		while (*text) {
			utf8charlen = utf8decode(text, &utf8codepoint, &utf8err);
			for (curfont = drw->fonts; curfont; curfont = curfont->next) {
				charexists = charexists || XftCharExists(drw->dpy, curfont->xfont, utf8codepoint);
				if (charexists) {
					drw_font_getexts(curfont, text, utf8charlen, &tmpw, NULL);
					if (ew + ellipsis_width <= w) {
						/* keep track where the ellipsis still fits */
						ellipsis_x = x + ew;
						ellipsis_w = w - ew;
						ellipsis_len = utf8strlen;
					}

					if (ew + tmpw > w) {
						overflow = 1;
						/* called from drw_fontset_getwidth_clamp():
						 * it wants the width AFTER the overflow
						 */
						if (!render)
							x += tmpw;
						else
							utf8strlen = ellipsis_len;
					} else if (curfont == usedfont) {
						text += utf8charlen;
						utf8strlen += utf8err ? 0 : utf8charlen;
						ew += utf8err ? 0 : tmpw;
					} else {
						nextfont = curfont;
					}
					break;
				}
			}

			if (overflow || !charexists || nextfont || utf8err)
				break;
			else
				charexists = 0;
		}

		if (utf8strlen) {
			if (render) {
				ty = y + (h - usedfont->h) / 2 + usedfont->xfont->ascent;
				XftDrawStringUtf8(d, &drw->scheme[invert ? ColBg : ColFg],
				                  usedfont->xfont, x, ty, (XftChar8 *)utf8str, utf8strlen);
			}
			x += ew;
			w -= ew;
		}
		if (utf8err && (!render || invalid_width < w)) {
			if (render)
				drw_text(drw, x, y, w, h, 0, invalid, invert);
			x += invalid_width;
			w -= invalid_width;
		}
		if (render && overflow)
			drw_text(drw, ellipsis_x, y, ellipsis_w, h, 0, "...", invert);

		if (!*text || overflow) {
			break;
		} else if (nextfont) {
			charexists = 0;
			usedfont = nextfont;
		} else {
			/* Regardless of whether or not a fallback font is found, the
			 * character must be drawn. */
			charexists = 1;

			hash = (unsigned int)utf8codepoint;
			hash = ((hash >> 16) ^ hash) * 0x21F0AAAD;
			hash = ((hash >> 15) ^ hash) * 0xD35A2D97;
			h0 = ((hash >> 15) ^ hash) % LENGTH(nomatches);
			h1 = (hash >> 17) % LENGTH(nomatches);
			/* avoid expensive XftFontMatch call when we know we won't find a match */
			if (nomatches[h0] == utf8codepoint || nomatches[h1] == utf8codepoint)
				goto no_match;

			fccharset = FcCharSetCreate();
			FcCharSetAddChar(fccharset, utf8codepoint);

			if (!drw->fonts->pattern) {
				/* Refer to the comment in xfont_create for more information. */
				die("the first font in the cache must be loaded from a font string.");
			}

			fcpattern = FcPatternDuplicate(drw->fonts->pattern);
			FcPatternAddCharSet(fcpattern, FC_CHARSET, fccharset);
			FcPatternAddBool(fcpattern, FC_SCALABLE, FcTrue);

			FcConfigSubstitute(NULL, fcpattern, FcMatchPattern);
			FcDefaultSubstitute(fcpattern);
			match = XftFontMatch(drw->dpy, drw->screen, fcpattern, &result);

			FcCharSetDestroy(fccharset);
			FcPatternDestroy(fcpattern);

			if (match) {
				usedfont = xfont_create(drw, NULL, match);
				if (usedfont && XftCharExists(drw->dpy, usedfont->xfont, utf8codepoint)) {
					for (curfont = drw->fonts; curfont->next; curfont = curfont->next)
						; /* NOP */
					curfont->next = usedfont;
				} else {
					xfont_free(usedfont);
					nomatches[nomatches[h0] ? h1 : h0] = utf8codepoint;
no_match:
					usedfont = drw->fonts;
				}
			}
		}
	}
	if (d)
		XftDrawDestroy(d);

	return x + (render ? w : 0);
}

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
void
drw_map(Drw *drw, Window win, int x, int y, unsigned int w, unsigned int h)
{
	if (!drw)
		return;

	XCopyArea(drw->dpy, drw->drawable, win, drw->gc, x, y, w, h, x, y);
	XSync(drw->dpy, False);
}

/**
 * @brief Get width of text with current font set
 * 
 * @param drw Drawing context
 * @param text Text to measure
 * @return Width in pixels
 */
unsigned int
drw_fontset_getwidth(Drw *drw, const char *text)
{
	if (!drw || !drw->fonts || !text)
		return 0;
	return drw_text(drw, 0, 0, 0, 0, 0, text, 0);
}

/**
 * @brief Get width of text with current font set (with length limit)
 * 
 * @param drw Drawing context
 * @param text Text to measure
 * @param n Maximum number of characters
 * @return Width in pixels
 */
unsigned int
drw_fontset_getwidth_clamp(Drw *drw, const char *text, unsigned int n)
{
	unsigned int tmp = 0;
	if (drw && drw->fonts && text && n)
		tmp = drw_text(drw, 0, 0, 0, 0, 0, text, n);
	return MIN(n, tmp);
}

/**
 * @brief Get text extents for a font
 * 
 * @param font Font to use
 * @param text Text to measure
 * @param len Length of text
 * @param w Width result
 * @param h Height result
 */
void
drw_font_getexts(Fnt *font, const char *text, unsigned int len, unsigned int *w, unsigned int *h)
{
	XGlyphInfo ext;

	if (!font || !text)
		return;

	XftTextExtentsUtf8(font->dpy, font->xfont, (XftChar8 *)text, len, &ext);
	if (w)
		*w = ext.xOff;
	if (h)
		*h = font->h;
}

/**
 * @brief Create a cursor
 * 
 * @param drw Drawing context
 * @param shape Cursor shape
 * @return Cursor
 */
Cur *
drw_cur_create(Drw *drw, int shape)
{
	Cur *cur;

	if (!drw || !(cur = ecalloc(1, sizeof(Cur))))
		return NULL;

	cur->cursor = XCreateFontCursor(drw->dpy, shape);

	return cur;
}

/**
 * @brief Free a cursor
 * 
 * @param drw Drawing context
 * @param cursor Cursor to free
 */
void
drw_cur_free(Drw *drw, Cur *cursor)
{
	if (!cursor)
		return;

	XFreeCursor(drw->dpy, cursor->cursor);
	free(cursor);
}