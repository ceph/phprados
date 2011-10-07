#ifndef PHP_RADOS_H
#define PHP_RADOS_H

#define PHP_RADOS_EXTNAME  "rados"
#define PHP_RADOS_EXTVER   "0.9.1"

extern "C" {
#include "php.h"
#include "php_ini.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include <rados/librados.h>
#ifdef ZTS
#include "TSRM.h"
#endif
}

#include <rados/librados.hpp>
using namespace librados;

/** FIXME Should correspond with librados! */
#define PHP_RADOS_POOL_MAX_LENGTH 128
#define PHP_RADOS_MAX_OBJECTS 1024
#define PHP_RADOS_CLUSTER_RES_NAME "RADOS Cluster"

struct rados_object
{
    zend_object std;
    Rados *rados;
    bool initialized;
    std::vector<const char*> argv;

    rados_object() :
    initialized(false),
    argv(NULL)
    {}
};

typedef struct _php_rados_cluster {
    rados_t cluster;
} php_rados_cluster;

PHP_MINIT_FUNCTION(rados);
PHP_MSHUTDOWN_FUNCTION(rados);
PHP_MINFO_FUNCTION(rados);

PHP_METHOD(Rados, __construct);
PHP_METHOD(Rados, init);
PHP_METHOD(Rados, connect);
PHP_METHOD(Rados, conf_read_file);
PHP_METHOD(Rados, conf_set);
PHP_METHOD(Rados, conf_get);
PHP_METHOD(Rados, shutdown);
PHP_METHOD(Rados, pool_create);
PHP_METHOD(Rados, pool_lookup);
PHP_METHOD(Rados, pool_delete);
PHP_METHOD(Rados, pool_list);
PHP_METHOD(Rados, get_pool_stats);
PHP_METHOD(Rados, cluster_stat);
PHP_METHOD(Rados, ioctx_create);

PHP_FUNCTION(rados_create);

extern zend_module_entry rados_module_entry;
#define phpext_rados_ptr &rados_module_entry;

extern const zend_function_entry php_rados_ioctx_methods[];

#endif
