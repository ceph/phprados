#ifndef PHP_RADOS_H
#define PHP_RADOS_H

#define PHP_RADOS_EXTNAME  "rados"
#define PHP_RADOS_EXTVER   "0.9.1"

#include "php.h"
#include "php_ini.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include <rados/librados.h>
#ifdef ZTS
#include "TSRM.h"
#endif

/** FIXME Should correspond with librados! */
#define PHP_RADOS_POOL_MAX_LENGTH 128
#define PHP_RADOS_MAX_OBJECTS 1024
#define PHP_RADOS_CLUSTER_RES_NAME "RADOS Cluster"

typedef struct _php_rados_cluster {
    rados_t cluster;
} php_rados_cluster;

PHP_MINIT_FUNCTION(rados);
PHP_MSHUTDOWN_FUNCTION(rados);
PHP_MINFO_FUNCTION(rados);

PHP_FUNCTION(rados_create);

extern zend_module_entry rados_module_entry;
#define phpext_rados_ptr &rados_module_entry;

extern const zend_function_entry php_rados_ioctx_methods[];

#endif
