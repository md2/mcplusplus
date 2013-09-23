#ifndef _I18N_H_
#define _I18N_H_

#ifdef HAVE_GETTEXT
#include <libintl.h>
#define _(msg) gettext(msg)
#else
#define _(msg) (msg)
#define textdomain(arg)
#define bindtextdomain(arg1,arg2)
#endif

#endif

