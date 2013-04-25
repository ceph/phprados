#ifndef PHP_RADOS_H
#define PHP_RADOS_H

#define PHP_RADOS_EXTNAME  "rados"
#define PHP_RADOS_EXTVER   "0.9.4-002"

#include "php.h"
#include "php_ini.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include "rados/librados.h"
#include <stdbool.h>
#ifdef ZTS
#include "TSRM.h"
#endif

#define PHP_RADOS_SNAP_NAME_MAX_LENGTH 64
#define PHP_RADOS_SNAP_MAX_NUM 256
#define PHP_RADOS_POOL_NAME_MAX_LENGTH 128
#define PHP_RADOS_OID_NAME_MAX_LENGTH 256
#define PHP_RADOS_CLUSTER_RES_NAME "RADOS Cluster"
#define PHP_RADOS_IOCTX_RES_NAME "RADOS IOCtx"

#endif
