/* LUTL - A simple Lua unit testing library. 
 * AUTHOR: Baptiste "Saend" CEILLIER
 * OVERVIEW:
 * The following functions are defined:
 *	lutl.verbosity(verbosity)
 *	lutl.reset()
 *
 * 	lutl.message(verbosity, msg, [showsrc=false])
 *	lutl.info(msg)
 *	lutl.warn(msg)
 *
 *	lutl.fail(msg)
 *
 * 	lutl.assert(val, [message])
 *
 * 	lutl.run(name, func, ...)
 *	lutl.test(name, ...)
 *	lutl.suite(name, ...)
 *
 *	lutl.summary()
 */
#pragma once

#include "cutl.h"

void lutl_dofile(cutl_t *cutl, const char *filename);
