#ifndef PHP_RADOS_H
#define PHP_RADOS_H

#define PHP_RADOS_EXTNAME  "rados"
#define PHP_RADOS_EXTVER   "0.9.3"

#include "php.h"
#include "php_ini.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include "rados/librados.h"
#ifdef ZTS
#include "TSRM.h"
#endif

#define PHP_RADOS_SNAP_NAME_MAX_LENGTH 64
#define PHP_RADOS_SNAP_MAX_NUM 256
#define PHP_RADOS_POOL_NAME_MAX_LENGTH 128
#define PHP_RADOS_OID_NAME_MAX_LENGTH 256
#define PHP_RADOS_CLUSTER_RES_NAME "RADOS Cluster"
#define PHP_RADOS_IOCTX_RES_NAME "RADOS IOCtx"

// We will register our streams as rados://
#define PHP_RADOS_STREAM_WRAPPER "rados"

typedef struct _php_rados_cluster {
    rados_t cluster;
} php_rados_cluster;

typedef struct _php_rados_ioctx {
    rados_ioctx_t io;
} php_rados_ioctx;

PHP_MINIT_FUNCTION(rados);
PHP_MSHUTDOWN_FUNCTION(rados);
PHP_MINFO_FUNCTION(rados);

PHP_FUNCTION(rados_create);
PHP_FUNCTION(rados_shutdown);
PHP_FUNCTION(rados_connect);
PHP_FUNCTION(rados_conf_read_file);
PHP_FUNCTION(rados_conf_set);
PHP_FUNCTION(rados_conf_get);
PHP_FUNCTION(rados_ioctx_create);
PHP_FUNCTION(rados_ioctx_destroy);
PHP_FUNCTION(rados_pool_list);
PHP_FUNCTION(rados_pool_lookup);
PHP_FUNCTION(rados_pool_create);
PHP_FUNCTION(rados_pool_delete);
PHP_FUNCTION(rados_ioctx_pool_set_auid);
PHP_FUNCTION(rados_ioctx_pool_get_auid);
PHP_FUNCTION(rados_write);
PHP_FUNCTION(rados_write_full);
PHP_FUNCTION(rados_read);
PHP_FUNCTION(rados_remove);
PHP_FUNCTION(rados_trunc);
PHP_FUNCTION(rados_append);
PHP_FUNCTION(rados_clone_range);
PHP_FUNCTION(rados_getxattr);
PHP_FUNCTION(rados_setxattr);
PHP_FUNCTION(rados_rmxattr);
PHP_FUNCTION(rados_stat);
PHP_FUNCTION(rados_get_last_version);
PHP_FUNCTION(rados_getxattrs);
PHP_FUNCTION(rados_objects_list);
PHP_FUNCTION(rados_ioctx_snap_create);
PHP_FUNCTION(rados_ioctx_snap_remove);
PHP_FUNCTION(rados_rollback);
PHP_FUNCTION(rados_ioctx_snap_list);
PHP_FUNCTION(rados_ioctx_snap_lookup);
PHP_FUNCTION(rados_ioctx_snap_get_name);
PHP_FUNCTION(rados_ioctx_snap_get_stamp);

extern zend_module_entry rados_module_entry;
#define phpext_rados_ptr &rados_module_entry;

static php_stream* rados_wrapper_open_url(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
static php_stream* rados_wrapper_open_dir(php_stream_wrapper *wrapper, char *filename, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
static int rados_wrapper_rename(php_stream_wrapper *wrapper, char *url_from, char *url_to, int options, php_stream_context *context TSRMLS_DC);
static int rados_wrapper_unlink(php_stream_wrapper *wrapper, char *url, int options, php_stream_context *context TSRMLS_DC);
static int rados_wrapper_stat(php_stream_wrapper *wrapper, char *url, int flags, php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC);
static int rados_wrapper_mkdir(php_stream_wrapper *wrapper, char *url, int mode, int options, php_stream_context *context TSRMLS_DC);
static int rados_wrapper_rmdir(php_stream_wrapper *wrapper, char *url, int options, php_stream_context *context TSRMLS_DC);

static size_t rados_stream_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC);
static size_t rados_stream_read( php_stream *stream, char *buf, size_t count TSRMLS_DC);
static int    rados_stream_close(php_stream *stream, int close_handle TSRMLS_DC);
static int    rados_stream_flush(php_stream *stream TSRMLS_DC);
static int    rados_stream_seek( php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC);
static int    rados_stream_stat( php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC);


#endif
