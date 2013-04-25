/*
 * phprados - A PHP5 extension for using librados
 *
 * Copyright (C) 2011 Wido den Hollander <wido@widodh.nl>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_rados.h"

int le_rados_cluster;
int le_rados_ioctx;

ZEND_BEGIN_ARG_INFO_EX(arginfo_rados_create, 0, 0, 0)
    ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_shutdown, 0)
    ZEND_ARG_INFO(0, cluster)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_connect, 0)
    ZEND_ARG_INFO(0, cluster)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_conf_read_file, 0)
    ZEND_ARG_INFO(0, cluster)
    ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_conf_set, 0)
    ZEND_ARG_INFO(0, cluster)
    ZEND_ARG_INFO(0, option)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_conf_get, 0)
    ZEND_ARG_INFO(0, cluster)
    ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_ioctx_create, 0)
    ZEND_ARG_INFO(0, cluster)
    ZEND_ARG_INFO(0, pool)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rados_pool_list, 0, 0, 1)
    ZEND_ARG_INFO(0, cluster)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_ioctx_destroy, 0)
    ZEND_ARG_INFO(0, ioctx)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_pool_lookup, 0)
    ZEND_ARG_INFO(0, cluster)
    ZEND_ARG_INFO(0, pool)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rados_pool_create, 0, 0, 2)
    ZEND_ARG_INFO(0, cluster)
    ZEND_ARG_INFO(0, pool)
    ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_pool_delete, 0)
    ZEND_ARG_INFO(0, cluster)
    ZEND_ARG_INFO(0, pool)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_ioctx_pool_set_auid, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, auid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_ioctx_pool_get_auid, 0)
    ZEND_ARG_INFO(0, ioctx)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rados_write, 0, 0, 3)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, oid)
    ZEND_ARG_INFO(0, buffer)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_write_full, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, oid)
    ZEND_ARG_INFO(0, buffer)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rados_read, 0, 0, 3)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, oid)
    ZEND_ARG_INFO(0, size)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_remove, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, oid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rados_trunc, 0, 0, 2)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, oid)
    ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_append, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, oid)
    ZEND_ARG_INFO(0, buffer)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_clone_range, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, dst_oidoid)
    ZEND_ARG_INFO(0, dst_offset)
    ZEND_ARG_INFO(0, src_offsetobj)
    ZEND_ARG_INFO(0, src_offset)
    ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_getxattr, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, oid)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_setxattr, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, oid)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_rmxattr, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, oid)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_stat, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, oid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_get_last_version, 0)
    ZEND_ARG_INFO(0, ioctx)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_getxattrs, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, oid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_objects_list, 0)
    ZEND_ARG_INFO(0, ioctx)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_ioctx_snap_create, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, snapname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_ioctx_snap_remove, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, snapname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_rollback, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, oid)
    ZEND_ARG_INFO(0, snapname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rados_ioctx_snap_list, 0, 0, 1)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, maxsnaps)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_ioctx_snap_lookup, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, snapid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_ioctx_snap_get_name, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, snapid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_ioctx_snap_get_stamp, 0)
    ZEND_ARG_INFO(0, ioctx)
    ZEND_ARG_INFO(0, snapid)
ZEND_END_ARG_INFO()

const zend_function_entry rados_functions[] = {
    PHP_FE(rados_create, arginfo_rados_create)
    PHP_FE(rados_shutdown, arginfo_rados_shutdown)
    PHP_FE(rados_connect, arginfo_rados_connect)
    PHP_FE(rados_conf_read_file, arginfo_rados_conf_read_file)
    PHP_FE(rados_conf_set, arginfo_rados_conf_set)
    PHP_FE(rados_conf_get, arginfo_rados_conf_get)
    PHP_FE(rados_ioctx_create, arginfo_rados_ioctx_create)
    PHP_FE(rados_ioctx_destroy, arginfo_rados_ioctx_destroy)
    PHP_FE(rados_pool_list, arginfo_rados_pool_list)
    PHP_FE(rados_pool_lookup, arginfo_rados_pool_lookup)
    PHP_FE(rados_pool_create, arginfo_rados_pool_create)
    PHP_FE(rados_pool_delete, arginfo_rados_pool_delete)
    PHP_FE(rados_ioctx_pool_set_auid, arginfo_rados_ioctx_pool_set_auid)
    PHP_FE(rados_ioctx_pool_get_auid, arginfo_rados_ioctx_pool_get_auid)
    PHP_FE(rados_write, arginfo_rados_write)
    PHP_FE(rados_write_full, arginfo_rados_write_full)
    PHP_FE(rados_read, arginfo_rados_read)
    PHP_FE(rados_remove, arginfo_rados_remove)
    PHP_FE(rados_trunc, arginfo_rados_trunc)
    PHP_FE(rados_append, arginfo_rados_append)
    PHP_FE(rados_clone_range, arginfo_rados_clone_range)
    PHP_FE(rados_getxattr, arginfo_rados_getxattr)
    PHP_FE(rados_setxattr, arginfo_rados_setxattr)
    PHP_FE(rados_rmxattr, arginfo_rados_rmxattr)
    PHP_FE(rados_stat, arginfo_rados_stat)
    PHP_FE(rados_get_last_version, arginfo_rados_get_last_version)
    PHP_FE(rados_getxattrs, arginfo_rados_getxattrs)
    PHP_FE(rados_objects_list, arginfo_rados_objects_list)
    PHP_FE(rados_ioctx_snap_create, arginfo_rados_ioctx_snap_create)
    PHP_FE(rados_ioctx_snap_remove, arginfo_rados_ioctx_snap_remove)
    PHP_FE(rados_rollback, arginfo_rados_rollback)
    PHP_FE(rados_ioctx_snap_list, arginfo_rados_ioctx_snap_list)
    PHP_FE(rados_ioctx_snap_lookup, arginfo_rados_ioctx_snap_lookup)
    PHP_FE(rados_ioctx_snap_get_name, arginfo_rados_ioctx_snap_get_name)
    PHP_FE(rados_ioctx_snap_get_stamp, arginfo_rados_ioctx_snap_get_stamp)
    {NULL, NULL, NULL}
};

/* Prevent us from calling functions when not connected or vise-versa */
static void verifyConnectionState(bool state, bool required_state) {
    if (state && !required_state) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "This function can't be called in a connected state");
    }

    if (!state && required_state) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "This function can't be called when not connected");
    }
}

/* librados C API */
PHP_FUNCTION(rados_create)
{
    php_rados_cluster *cluster_r;
    rados_t cluster;
    char *id = NULL;
    int id_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &id, &id_len) == FAILURE) {
        RETURN_NULL();
    }

    if (rados_create(&cluster, id) < 0) {
        RETURN_FALSE;
    }

    cluster_r = (php_rados_cluster *)emalloc(sizeof(php_rados_cluster));
    cluster_r->cluster = cluster;
    cluster_r->connected = false;
    ZEND_REGISTER_RESOURCE(return_value, cluster_r, le_rados_cluster);
}

PHP_FUNCTION(rados_shutdown)
{
    php_rados_cluster *cluster_r;
    zval *zcluster;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zcluster) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(cluster_r, php_rados_cluster*, &zcluster, -1, PHP_RADOS_CLUSTER_RES_NAME, le_rados_cluster);

    rados_shutdown(cluster_r->cluster);
    cluster_r->connected = false;

    RETURN_TRUE;
}

PHP_FUNCTION(rados_connect)
{
    php_rados_cluster *cluster_r;
    zval *zcluster;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zcluster) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(cluster_r, php_rados_cluster*, &zcluster, -1, PHP_RADOS_CLUSTER_RES_NAME, le_rados_cluster);

    if (rados_connect(cluster_r->cluster) < 0) {
        RETURN_FALSE;
    }

    cluster_r->connected = true;

    RETURN_TRUE;
}

PHP_FUNCTION(rados_conf_read_file)
{
    php_rados_cluster *cluster_r;
    zval *zcluster;
    char *path = NULL;
    int path_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zcluster, &path, &path_len) == FAILURE) {
        RETURN_NULL();
    }

    ZEND_FETCH_RESOURCE(cluster_r, php_rados_cluster*, &zcluster, -1, PHP_RADOS_CLUSTER_RES_NAME, le_rados_cluster);

    verifyConnectionState(cluster_r->connected, false);

    if (rados_conf_read_file(cluster_r->cluster, path) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_conf_set)
{
    php_rados_cluster *cluster_r;
    zval *zcluster;
    char *option, *value = NULL;
    int option_len, value_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &zcluster, &option, &option_len, &value, &value_len) == FAILURE) {
        RETURN_NULL();
    }

    ZEND_FETCH_RESOURCE(cluster_r, php_rados_cluster*, &zcluster, -1, PHP_RADOS_CLUSTER_RES_NAME, le_rados_cluster);

    verifyConnectionState(cluster_r->connected, false);

    if (rados_conf_set(cluster_r->cluster, option, value) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_conf_get)
{
    php_rados_cluster *cluster_r;
    zval *zcluster;
    char *option = NULL;
    int option_len = 0;
    char value[256];

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zcluster, &option, &option_len) == FAILURE) {
        RETURN_NULL();
    }

    ZEND_FETCH_RESOURCE(cluster_r, php_rados_cluster*, &zcluster, -1, PHP_RADOS_CLUSTER_RES_NAME, le_rados_cluster);

    if (rados_conf_get(cluster_r->cluster, option, value, sizeof(value)) < 0) {
        RETURN_FALSE;
    }

    RETURN_STRINGL(value, strlen(value), 1);
}

PHP_FUNCTION(rados_ioctx_create)
{
    php_rados_cluster *cluster_r;
    php_rados_ioctx *ioctx_r;
    zval *zcluster;
    rados_ioctx_t io;
    char *pool = NULL;
    int pool_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zcluster, &pool, &pool_len) == FAILURE) {
        RETURN_NULL();
    }

    ZEND_FETCH_RESOURCE(cluster_r, php_rados_cluster*, &zcluster, -1, PHP_RADOS_CLUSTER_RES_NAME, le_rados_cluster);

    verifyConnectionState(cluster_r->connected, true);

    if (rados_ioctx_create(cluster_r->cluster, pool, &io) < 0) {
        RETURN_FALSE;
    }

    ioctx_r = (php_rados_ioctx *)emalloc(sizeof(php_rados_ioctx));
    ioctx_r->io = io;
    ZEND_REGISTER_RESOURCE(return_value, ioctx_r, le_rados_ioctx);
}

PHP_FUNCTION(rados_ioctx_destroy)
{
    php_rados_ioctx *ioctx_r;
    zval *zioctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zioctx) == FAILURE) {
        RETURN_NULL();
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    rados_ioctx_destroy(ioctx_r->io);
}

PHP_FUNCTION(rados_pool_list)
{
    php_rados_cluster *cluster_r;
    zval *zcluster;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zcluster) == FAILURE) {
        RETURN_NULL();
    }

    ZEND_FETCH_RESOURCE(cluster_r, php_rados_cluster*, &zcluster, -1, PHP_RADOS_CLUSTER_RES_NAME, le_rados_cluster);

    verifyConnectionState(cluster_r->connected, true);

    char temp_buff[256];
    int buff_size = rados_pool_list(cluster_r->cluster, temp_buff, 0);

    char buff[buff_size];
    int r = rados_pool_list(cluster_r->cluster, buff, sizeof(buff));
    if (r != buff_size) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Buffer size mismatch: Got %d, expected %d", r, buff_size);
        RETURN_FALSE;
    }

    array_init(return_value);
    const char *b = buff;
    while (1) {
        if (b[0] == '\0') {
            break;
        }

        add_next_index_string(return_value, b, 1);
        b += strlen(b) + 1;
    }
}

PHP_FUNCTION(rados_pool_lookup)
{
    php_rados_cluster *cluster_r;
    zval *zcluster;
    char *pool = NULL;
    int pool_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zcluster, &pool, &pool_len) == FAILURE) {
        RETURN_NULL();
    }

    ZEND_FETCH_RESOURCE(cluster_r, php_rados_cluster*, &zcluster, -1, PHP_RADOS_CLUSTER_RES_NAME, le_rados_cluster);

    verifyConnectionState(cluster_r->connected, true);

    if (rados_pool_lookup(cluster_r->cluster, pool) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_pool_create)
{
    zval *zcluster;
    zval *options;
    zval **entry;
    php_rados_cluster *cluster_r;
    char *pool = NULL;
    char *key;
    uint key_len;
    int auid = NULL;
    int crushrule = NULL;
    int pool_len = 0;
    ulong option;
    HashPosition pos;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|a", &zcluster, &pool, &pool_len, &options) == FAILURE) {
        RETURN_NULL();
    }

    if (ZEND_NUM_ARGS() == 3 && Z_TYPE_P(options) == IS_ARRAY) {
        zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(options), &pos);
        while (zend_hash_get_current_data_ex(Z_ARRVAL_P(options), (void **)&entry, &pos) == SUCCESS) {
            if (zend_hash_get_current_key_ex(Z_ARRVAL_P(options), &key, &key_len, &option, 0, &pos) != HASH_KEY_IS_STRING) {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Array keys must be strings");
                RETURN_NULL();
            }

            if (Z_TYPE_PP(entry) == IS_LONG) {
                if (strcmp(key, "auid") == 0) {
                    auid = Z_LVAL_PP(entry);
                }

                if (strcmp(key, "crushrule") == 0) {
                    crushrule = Z_LVAL_PP(entry);
                }
            }

            zend_hash_move_forward_ex(Z_ARRVAL_P(options), &pos);
        }
    }

    ZEND_FETCH_RESOURCE(cluster_r, php_rados_cluster*, &zcluster, -1, PHP_RADOS_CLUSTER_RES_NAME, le_rados_cluster);

    verifyConnectionState(cluster_r->connected, true);

    int r;
    if ((auid != NULL) && (crushrule != NULL)) {
        r = rados_pool_create_with_all(cluster_r->cluster, pool, auid, crushrule);
    } else if (auid != NULL) {
        r = rados_pool_create_with_auid(cluster_r->cluster, pool, auid);
    } else if (crushrule != NULL) {
        r = rados_pool_create_with_crush_rule(cluster_r->cluster, pool, crushrule);
    } else  {
        r = rados_pool_create(cluster_r->cluster, pool);
    }

    if (r < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_pool_delete)
{
    php_rados_cluster *cluster_r;
    zval *zcluster;
    char *pool = NULL;
    int pool_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zcluster, &pool, &pool_len) == FAILURE) {
        RETURN_NULL();
    }

    ZEND_FETCH_RESOURCE(cluster_r, php_rados_cluster*, &zcluster, -1, PHP_RADOS_CLUSTER_RES_NAME, le_rados_cluster);

    verifyConnectionState(cluster_r->connected, true);

    if (rados_pool_delete(cluster_r->cluster, pool) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_ioctx_pool_set_auid)
{
    php_rados_ioctx *ioctx_r;
    zval *zioctx;
    int auid;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zioctx, &auid) == FAILURE) {
        RETURN_NULL();
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_ioctx_pool_set_auid(ioctx_r->io, auid) < 0) {
        RETURN_FALSE;
    } else {
        RETURN_TRUE;
    }
}

PHP_FUNCTION(rados_ioctx_pool_get_auid)
{
    php_rados_ioctx *ioctx_r;
    zval *zioctx;
    uint64_t auid;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zioctx) == FAILURE) {
        RETURN_NULL();
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_ioctx_pool_get_auid(ioctx_r->io, &auid) < 0) {
        RETURN_FALSE;
    }

    RETURN_LONG(auid);
}

PHP_FUNCTION(rados_write) {
    php_rados_ioctx *ioctx_r;
    char *oid=NULL;
    char *buffer=NULL;
    int oid_len;
    size_t buffer_len;
    uint64_t offset = 0;
    zval *zioctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|l", &zioctx, &oid, &oid_len, &buffer, &buffer_len, &offset) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_write(ioctx_r->io, oid, buffer, buffer_len, offset) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_write_full) {
    php_rados_ioctx *ioctx_r;
    char *oid=NULL;
    char *buffer=NULL;
    int oid_len;
    size_t buffer_len;
    zval *zioctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &zioctx, &oid, &oid_len, &buffer, &buffer_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_write_full(ioctx_r->io, oid, buffer, buffer_len) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_read) {
    php_rados_ioctx *ioctx_r;
    char *oid=NULL;
    int oid_len;
    size_t size;
    zval *zioctx;
    uint64_t offset = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl|l", &zioctx, &oid, &oid_len, &size, &offset) == FAILURE) {
        RETURN_FALSE;
    }

    char buffer[size];

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_read(ioctx_r->io, oid, buffer, size, offset) < 0) {
        RETURN_FALSE;
    }

    RETURN_STRINGL(buffer, size, 1);
}

PHP_FUNCTION(rados_remove) {
    php_rados_ioctx *ioctx_r;
    char *oid=NULL;
    int oid_len;
    zval *zioctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zioctx, &oid, &oid_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_remove(ioctx_r->io, oid) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_trunc) {
    php_rados_ioctx *ioctx_r;
    char *oid=NULL;
    int oid_len;
    size_t size;
    zval *zioctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl|l", &zioctx, &oid, &oid_len, &size) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_trunc(ioctx_r->io, oid, size) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_append) {
    php_rados_ioctx *ioctx_r;
    char *oid=NULL;
    char *buffer=NULL;
    int oid_len;
    size_t buffer_len;
    zval *zioctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|l", &zioctx, &oid, &oid_len, &buffer, &buffer_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_append(ioctx_r->io, oid, buffer, buffer_len) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_clone_range) {
    php_rados_ioctx *ioctx_r;
    char *dst_oid=NULL;
    char *src_oid=NULL;
    int dst_oid_len;
    int src_oid_len;
    size_t size;
    uint64_t dst_offset;
    uint64_t src_offset;
    zval *zioctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rslsll", &zioctx, &dst_oid, &dst_oid_len, &dst_offset, &src_oid, &src_oid_len, &src_offset, &size) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_clone_range(ioctx_r->io, dst_oid, dst_offset, src_oid, src_offset, size) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_getxattr) {
    php_rados_ioctx *ioctx_r;
    char *oid=NULL;
    char *name=NULL;
    int oid_len;
    int name_len;
    size_t size;
    zval *zioctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rssl", &zioctx, &oid, &oid_len, &name, &name_len, &size) == FAILURE) {
        RETURN_FALSE;
    }

    char buffer[size];

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_getxattr(ioctx_r->io, oid, name, buffer, size) < 0) {
        RETURN_FALSE;
    }

    RETURN_STRINGL(buffer, size, 1);
}

PHP_FUNCTION(rados_setxattr) {
    php_rados_ioctx *ioctx_r;
    char *oid=NULL;
    char *name=NULL;
    char *buffer=NULL;
    int oid_len;
    int name_len;
    size_t buffer_len;
    zval *zioctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsss", &zioctx, &oid, &oid_len, &name, &name_len, &buffer, &buffer_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_setxattr(ioctx_r->io, oid, name, buffer, buffer_len) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_rmxattr) {
    php_rados_ioctx *ioctx_r;
    char *oid=NULL;
    char *name=NULL;
    int oid_len;
    int name_len;
    size_t size;
    zval *zioctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &zioctx, &oid, &oid_len, &name, &name_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_rmxattr(ioctx_r->io, oid, name) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_stat) {
    php_rados_ioctx *ioctx_r;
    char *oid=NULL;
    int oid_len;
    uint64_t psize;
    time_t pmtime;
    zval *zioctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zioctx, &oid, &oid_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_stat(ioctx_r->io, oid, &psize, &pmtime) < 0) {
        RETURN_FALSE;
    }

    array_init(return_value);
    add_assoc_string(return_value, "oid", oid, 1);
    add_assoc_long(return_value, "psize", (long)psize);
    add_assoc_long(return_value, "pmtime", (long)pmtime);
}

PHP_FUNCTION(rados_get_last_version) {
    php_rados_ioctx *ioctx_r;
    zval *zioctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zioctx) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    RETURN_LONG(rados_get_last_version(ioctx_r->io));
}

PHP_FUNCTION(rados_getxattrs) {
    php_rados_ioctx *ioctx_r;
    char *oid=NULL;
    int oid_len;
    zval *zioctx;
    rados_xattrs_iter_t iter;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zioctx, &oid, &oid_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    array_init(return_value);

    rados_getxattrs(ioctx_r->io, oid, &iter);
    while (1) {
        const char *name;
        const char *val;
        char *value;
        size_t len;

        rados_getxattrs_next(iter, &name, &val, &len);
        if (name == NULL) {
            break;
        }

		// Some tricks to convert const char *val to char *val and pass it to add_assoc_string_ex
		size_t length = strlen(val) + 1;
		char buffer[length]; 
		memcpy(buffer, val , length); 
        
        add_assoc_stringl_ex(return_value, name, strlen(name)+1, buffer, len, 1);
    }
    rados_getxattrs_end(iter);
}

PHP_FUNCTION(rados_objects_list) {
    php_rados_ioctx *ioctx_r;
    zval *zioctx;
    rados_list_ctx_t ctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zioctx) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    array_init(return_value);

    rados_objects_list_open(ioctx_r->io, &ctx);
    const char *oid;
    while (rados_objects_list_next(ctx, &oid, NULL) == 0) {
        add_next_index_string(return_value, oid, 1);
    }
    rados_objects_list_close(ctx);
}

PHP_FUNCTION(rados_ioctx_snap_create) {
    php_rados_ioctx *ioctx_r;
    zval *zioctx;
    char *snapname=NULL;
    int snapname_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zioctx, &snapname, &snapname_len) == FAILURE) {
        RETURN_FALSE;
    }

    if (snapname_len > PHP_RADOS_SNAP_NAME_MAX_LENGTH) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The snapshot name (%s) is too long!", snapname);
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_ioctx_snap_create(ioctx_r->io, snapname) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_ioctx_snap_remove) {
    php_rados_ioctx *ioctx_r;
    zval *zioctx;
    char *snapname=NULL;
    int snapname_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zioctx, &snapname, &snapname_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_ioctx_snap_remove(ioctx_r->io, snapname) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_rollback) {
    php_rados_ioctx *ioctx_r;
    zval *zioctx;
    char *snapname=NULL;
    int snapname_len;
    char *oid=NULL;
    int oid_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &zioctx, &oid, &oid_len, &snapname, &snapname_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_rollback(ioctx_r->io, oid, snapname) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(rados_ioctx_snap_list) {
    php_rados_ioctx *ioctx_r;
    zval *zioctx;
    rados_list_ctx_t ctx;
    int maxsnaps = PHP_RADOS_SNAP_MAX_NUM;
    int i, r;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zioctx, &maxsnaps) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (maxsnaps > PHP_RADOS_SNAP_MAX_NUM) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "The maximum number of snapshots per pool is %d!", PHP_RADOS_SNAP_MAX_NUM);
        RETURN_FALSE;
    }

    rados_snap_t snaps[maxsnaps];

    r = rados_ioctx_snap_list(ioctx_r->io, snaps, maxsnaps);
    if (r < 0) {
        RETURN_FALSE;
    }

    array_init(return_value);
    for (i = 0; i < r; i++) {
        add_next_index_long(return_value, (long)snaps[i]);
    }
}


PHP_FUNCTION(rados_ioctx_snap_lookup) {
    php_rados_ioctx *ioctx_r;
    zval *zioctx;
    char *snapname=NULL;
    int snapname_len;
    rados_snap_t snapid;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zioctx, &snapname, &snapname_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_ioctx_snap_lookup(ioctx_r->io, snapname, &snapid) < 0) {
        RETURN_FALSE;
    }

    RETURN_LONG(snapid);
}

PHP_FUNCTION(rados_ioctx_snap_get_name) {
    php_rados_ioctx *ioctx_r;
    zval *zioctx;
    char snapname[PHP_RADOS_SNAP_NAME_MAX_LENGTH];
    long snapid;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zioctx, &snapid) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_ioctx_snap_get_name(ioctx_r->io, snapid, snapname, sizeof(snapname)) < 0) {
        RETURN_FALSE;
    }

    RETURN_STRINGL(snapname, strlen(snapname), 1);
}

PHP_FUNCTION(rados_ioctx_snap_get_stamp) {
    php_rados_ioctx *ioctx_r;
    zval *zioctx;
    time_t time;
    long snapid;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zioctx, &snapid) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(ioctx_r, php_rados_ioctx*, &zioctx, -1, PHP_RADOS_IOCTX_RES_NAME, le_rados_ioctx);

    if (rados_ioctx_snap_get_stamp(ioctx_r->io, snapid, &time) < 0) {
        RETURN_FALSE;
    }

    RETURN_LONG(time);
}
