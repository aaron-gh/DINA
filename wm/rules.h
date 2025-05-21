/**
 * @file rules.h
 * @brief Window rule functionality for DINA
 *
 * This file contains functions for applying rules to windows.
 */

#ifndef _RULES_H
#define _RULES_H

#include "../core/dina.h"

/**
 * @brief Apply rules to a client
 * 
 * Check window properties and apply matching rules
 * 
 * @param c Client to apply rules to
 */
void applyrules(Client *c);

#endif /* _RULES_H */