#ifndef PHP_RADOS_H
#define PHP_RADOS_H

#define PHP_RADOS_EXTNAME  "rados"
#define PHP_RADOS_EXTVER   "0.9.0"

extern "C" {
#include "php.h"
#include "php_ini.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#ifdef ZTS
#include "TSRM.h"
#endif
}

#include <string>
#include <sstream>
#include <rados/librados.hpp>
using namespace librados;

typedef struct _php_rados_pool {
    pool_t pool;
} php_rados_pool;

typedef struct _php_rados_listctx {
    Rados::ListCtx ctx;
} php_rados_listctx;

#define PHP_RADOS_POOL_RES_NAME "RADOS Pool"
#define PHP_RADOS_LISTCTX_RES_NAME "RADOS List CTX"
/** FIXME Should correspond with librados! */
#define PHP_RADOS_POOL_MAX_LENGTH 128
#define PHP_RADOS_MAX_OBJECTS 1024

PHP_MINIT_FUNCTION(rados);
PHP_MSHUTDOWN_FUNCTION(rados);
PHP_MINFO_FUNCTION(rados);

PHP_METHOD(Rados, __construct);
PHP_METHOD(Rados, initialize);

/* Pool methods */
PHP_METHOD(Rados, open_pool);
PHP_METHOD(Rados, close_pool);
PHP_METHOD(Rados, create_pool);
PHP_METHOD(Rados, delete_pool);
PHP_METHOD(Rados, lookup_pool);
PHP_METHOD(Rados, change_pool_auid);
PHP_METHOD(Rados, list_pools);
PHP_METHOD(Rados, snap_create);
PHP_METHOD(Rados, snap_remove);
PHP_METHOD(Rados, list_objects);
PHP_METHOD(Rados, list_objects_open);
PHP_METHOD(Rados, list_objects_more);
PHP_METHOD(Rados, list_objects_close);

/* Object methods */
PHP_METHOD(Rados, create);
PHP_METHOD(Rados, remove);
PHP_METHOD(Rados, stat);
PHP_METHOD(Rados, write_full);
PHP_METHOD(Rados, write);
PHP_METHOD(Rados, read);
PHP_METHOD(Rados, read_full);
PHP_METHOD(Rados, trunc);
PHP_METHOD(Rados, getxattr);
PHP_METHOD(Rados, setxattr);

extern zend_module_entry rados_module_entry;
#define phpext_rados_ptr &rados_module_entry;

#endif
