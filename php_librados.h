#ifndef PHP_LIBRADOS_H
#define PHP_LIBRADOS_H

#define PHP_LIBRADOS_EXTNAME  "librados"
#define PHP_LIBRADOS_EXTVER   "0.9"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include "php.h"
#ifdef ZTS
#include "TSRM.h"
#endif
}

extern zend_module_entry librados_module_entry;
#define phpext_librados_ptr &librados_module_entry;

#endif /* PHP_LIBRADOS_H */
