/**
 * @file event.h
 * @brief Event handling for DINA
 *
 * This file contains functions for handling X11 events in the DINA window manager.
 */

#ifndef _EVENT_H
#define _EVENT_H

#include <X11/Xlib.h>
#include "dina.h"

/* X protocol request codes */
#define X_SetInputFocus    42
#define X_PolyText8        74
#define X_PolyFillRectangle 70
#define X_PolySegment      66
#define X_ConfigureWindow  12
#define X_GrabButton       28
#define X_GrabKey          33
#define X_CopyArea         62

/**
 * @brief Update numlock mask
 * 
 * Update the mask for numlock modifier
 */
void updatenumlockmask(void);

/**
 * @brief Initialize event handling
 * 
 * Set up event handlers and grab keys
 */
void event_init(void);

/**
 * @brief Clean up event handling
 * 
 * Release resources used by event handling
 */
void event_cleanup(void);

/**
 * @brief Handle X events
 * 
 * Dispatch X events to appropriate handlers
 * 
 * @param e X event to handle
 */
void handle_event(XEvent *e);

/**
 * @brief Check for other window managers
 * 
 * Verify no other window manager is running
 */
void checkotherwm(void);

/**
 * @brief Grab keyboard keys
 * 
 * Set up key grabs for keyboard shortcuts
 */
void grabkeys(void);

/**
 * @brief Handle button press events
 * 
 * @param e X event
 */
void buttonpress(XEvent *e);

/**
 * @brief Handle client message events
 * 
 * @param e X event
 */
void clientmessage(XEvent *e);

/**
 * @brief Handle configure request events
 * 
 * @param e X event
 */
void configurerequest(XEvent *e);

/**
 * @brief Handle configure notify events
 * 
 * @param e X event
 */
void configurenotify(XEvent *e);

/**
 * @brief Handle destroy notify events
 * 
 * @param e X event
 */
void destroynotify(XEvent *e);

/**
 * @brief Handle enter notify events
 * 
 * @param e X event
 */
void enternotify(XEvent *e);

/**
 * @brief Handle expose events
 * 
 * @param e X event
 */
void expose(XEvent *e);

/**
 * @brief Handle focus in events
 * 
 * @param e X event
 */
void focusin(XEvent *e);

/**
 * @brief Handle key press events
 * 
 * @param e X event
 */
void keypress(XEvent *e);

/**
 * @brief Handle mapping notify events
 * 
 * @param e X event
 */
void mappingnotify(XEvent *e);

/**
 * @brief Handle map request events
 * 
 * @param e X event
 */
void maprequest(XEvent *e);

/**
 * @brief Handle motion notify events
 * 
 * @param e X event
 */
void motionnotify(XEvent *e);

/**
 * @brief Handle property notify events
 * 
 * @param e X event
 */
void propertynotify(XEvent *e);

/**
 * @brief Handle unmap notify events
 * 
 * @param e X event
 */
void unmapnotify(XEvent *e);

/**
 * @brief Send an event to a window
 * 
 * @param c Client to send event to
 * @param proto Atom representing the protocol
 * @return 1 if successful, 0 otherwise
 */
int sendevent(Client *c, Atom proto);

/**
 * @brief X error handler
 * 
 * @param dpy Display
 * @param ee X error event
 * @return Error code
 */
int xerror(Display *dpy, XErrorEvent *ee);

/**
 * @brief Dummy X error handler
 * 
 * @param dpy Display
 * @param ee X error event
 * @return Always 0
 */
int xerrordummy(Display *dpy, XErrorEvent *ee);

/**
 * @brief Startup X error handler
 * 
 * @param dpy Display
 * @param ee X error event
 * @return Error code
 */
int xerrorstart(Display *dpy, XErrorEvent *ee);

#endif /* _EVENT_H */