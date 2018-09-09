#ifndef __BASH_INCLUDES_H_
#define __BASH_INCLUDES_H_

/* copy loadables.h */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "builtins.h"
#include "shell.h"
#include "bashgetopt.h"
#include "common.h"
#include "hashlib.h"

/* maybe in the future use gettext */
#define _(msgid) msgid

#endif
