#ifndef PHP_RADOS_PR_H
#define PHP_RADOS_PR_H

typedef struct _php_rados_cluster {
    rados_t cluster;
    bool connected;
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

#endif
