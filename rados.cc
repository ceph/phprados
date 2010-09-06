/*
 * phprados - A PHP5 extension for using librados
 *
 * Copyright (C) 2010 Wido den Hollander <wido@widodh.nl>
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

zend_object_handlers rados_object_handlers;

zend_class_entry *rados_rados_ce;
zend_class_entry *rados_radosexception_ce;

int le_rados_pool;
int le_rados_listctx;

struct rados_object {
    zend_object std;
    Rados *rados;
    bool initialized;
    rados_object() {
      initialized = false;
    }
};

const zend_function_entry rados_rados_methods[] = {
    PHP_ME(Rados, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Rados, initialize, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, open_pool, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, close_pool, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, create_pool, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, delete_pool, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, lookup_pool, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, change_pool_auid, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, list_pools, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, snap_create, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, snap_remove, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, selfmanaged_snap_create, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, snap_rollback_object, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, list_objects, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, list_objects_open, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, list_objects_more, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, list_objects_close, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, create, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, remove, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, stat, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, write_full, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, write, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, read, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, read_full, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, trunc, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, getxattr, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, setxattr, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, getxattrs, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, get_pool_stats, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, get_fs_stats, NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

const zend_function_entry php_rados_radosexception_methods[] = {
    {NULL, NULL, NULL}
};

void rados_free_storage(void *object TSRMLS_DC)
{
    Rados *rados;
    rados_object *obj = (rados_object *)object;
    rados = obj->rados;
    if (rados != NULL && obj->initialized) {
        rados->shutdown();
    }
    delete obj->rados;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);

    efree(obj);
}

static char* uint642char(uint64_t u) {
    std::stringstream c_s;
    c_s << u;
    char *c_buf = (char *)emalloc(sizeof(c_s.str().c_str()));
    strcpy(c_buf, c_s.str().c_str());
    return c_buf;
};

zend_object_value rados_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    rados_object *obj = (rados_object *)emalloc(sizeof(rados_object));
    memset(obj, 0, sizeof(rados_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(obj->std.properties, &type->default_properties, (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));

    retval.handle = zend_objects_store_put(obj, NULL, rados_free_storage, NULL TSRMLS_CC);
    retval.handlers = &rados_object_handlers;

    return retval;
}


PHP_METHOD(Rados, __construct)
{
    Rados *rados = NULL;
    zval *object = getThis();

    rados = new Rados();
    rados_object *obj = (rados_object *)zend_object_store_get_object(object TSRMLS_CC);
    obj->rados = rados;
}

PHP_METHOD(Rados, initialize)
{
    Rados *rados;

    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (!obj->initialized) {

        int argc = 1;
        const char *argv[0];

        if (INI_STR("rados.monitor_ip") != NULL) {
            argv[argc++] = "-m";
            argv[argc++] = INI_STR("rados.monitor_ip");
        }

        if (INI_STR("rados.cephx_keyfile") != NULL) {
            argv[argc++] = "-K";
            argv[argc++] = INI_STR("rados.cephx_keyfile");
        }

        if (INI_STR("rados.cephx_keyring") != NULL) {
            argv[argc++] = "-k";
            argv[argc++] = INI_STR("rados.cephx_keyring");
        }

        if (INI_STR("rados.cephx_name") != NULL) {
            argv[argc++] = "-n";
            argv[argc++] = INI_STR("rados.cephx_name");
        }

        if (INI_STR("rados.config_file") != NULL) {
            argv[argc++] = "-c";
            argv[argc++] = INI_STR("rados.config_file");
        }

        rados = obj->rados;
        if (rados->initialize(argc, argv) < 0) {
            zend_throw_exception(rados_radosexception_ce, "Failed to initialize RADOS!", 0 TSRMLS_CC);
            return;
        }
        obj->initialized = true;
    }
    RETURN_TRUE;
}

PHP_METHOD(Rados, open_pool)
{
    pool_t pool;
    php_rados_pool *pool_r;

    char *spool=NULL;
    int spool_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &spool, &spool_len) == FAILURE) {
        RETURN_NULL();
    }

    if (spool_len > PHP_RADOS_POOL_MAX_LENGTH) {
        zend_throw_exception(rados_radosexception_ce, "The specified RADOS poolname is too long!", 0 TSRMLS_CC);
        return;
    }

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if(rados->open_pool(spool, &pool) < 0) {
        zend_throw_exception(rados_radosexception_ce, "The specified RADOS pool could not be opened.", 0 TSRMLS_CC);
        return;
    }

    pool_r = (php_rados_pool *)emalloc(sizeof(php_rados_pool));
    pool_r->pool = pool;
    ZEND_REGISTER_RESOURCE(return_value, pool_r, le_rados_pool);
}

PHP_METHOD(Rados, close_pool)
{
    php_rados_pool *pool_r;
    zval *zpool;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zpool) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if(rados->close_pool(pool_r->pool) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, create_pool)
{
    char *spool=NULL;
    int spool_len;
    int auid = 0;
    int crushrule = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ll", &spool, &spool_len, &auid, &crushrule) == FAILURE) {
        RETURN_NULL();
    }

    if (spool_len > PHP_RADOS_POOL_MAX_LENGTH) {
        zend_throw_exception(rados_radosexception_ce, "The specified RADOS poolname is too long!", 0 TSRMLS_CC);
        return;
    }

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if(rados->create_pool(spool, auid, crushrule) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, delete_pool)
{
    php_rados_pool *pool_r;
    zval *zpool;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zpool) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if(rados->delete_pool(pool_r->pool) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, lookup_pool)
{
    char *spool=NULL;
    int spool_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &spool, &spool_len) == FAILURE) {
        RETURN_NULL();
    }

    if (spool_len > PHP_RADOS_POOL_MAX_LENGTH) {
        zend_throw_exception(rados_radosexception_ce, "The specified RADOS poolname is too long!", 0 TSRMLS_CC);
        return;
    }

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados != NULL) {
        if(rados->lookup_pool(spool) < 0) {
            RETURN_FALSE;
        }
    } else {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, change_pool_auid)
{
    php_rados_pool *pool_r;
    zval *zpool;
    int auid;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zpool, &auid) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados->change_pool_auid(pool_r->pool, auid) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, list_pools)
{
    std::list<std::string> pools;

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if(rados->list_pools(pools) < 0) {
        RETURN_FALSE;
    }

    array_init(return_value);
    int j = 0;
    for (std::list<std::string>::iterator i = pools.begin(); i != pools.end(); i++) {
        add_next_index_string(return_value, i->c_str(), j++);
    }
}

PHP_METHOD(Rados, get_pool_stats)
{
    std::list<std::string> v;
    std::map<std::string,pool_stat_t> stats;
    zval *pools;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &pools) == FAILURE) {
        RETURN_FALSE;
    }

    if (Z_TYPE_P(pools) == IS_STRING) {
        v.push_back(Z_STRVAL_P(pools));
    } else if (Z_TYPE_P(pools) == IS_ARRAY) {
        HashTable *arr_hash;
        HashPosition hash_pos;
        zval **arr_value;
        arr_hash = Z_ARRVAL_P(pools);

        for(zend_hash_internal_pointer_reset_ex(arr_hash, &hash_pos); zend_hash_get_current_data_ex(arr_hash, (void**) &arr_value, &hash_pos) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &hash_pos)) {
            if (Z_TYPE_PP(arr_value) == IS_STRING) {
                v.push_back(Z_STRVAL_PP(arr_value));
            }
        }
    } else {
        RETURN_FALSE;
    }

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if(rados->get_pool_stats(v, stats) < 0) {
        RETURN_FALSE;
    }

    array_init(return_value);
    for (std::map<std::string,pool_stat_t>::iterator i = stats.begin(); i != stats.end(); i++) {
        zval *pool_stats;
        ALLOC_INIT_ZVAL(pool_stats);
        array_init(pool_stats);

        add_assoc_string(pool_stats, "num_bytes", uint642char(i->second.num_bytes), 0);
        add_assoc_string(pool_stats, "num_kb", uint642char(i->second.num_kb), 1);
        add_assoc_string(pool_stats, "num_objects", uint642char(i->second.num_objects), 2);
        add_assoc_string(pool_stats, "num_object_clones", uint642char(i->second.num_object_clones), 3);
        add_assoc_string(pool_stats, "num_object_copies", uint642char(i->second.num_object_copies), 4);
        add_assoc_string(pool_stats, "num_objects_missing_on_primary", uint642char(i->second.num_objects_missing_on_primary), 5);
        add_assoc_string(pool_stats, "num_objects_degraded", uint642char(i->second.num_objects_degraded), 6);
        add_assoc_string(pool_stats, "num_rd", uint642char(i->second.num_rd), 7);
        add_assoc_string(pool_stats, "num_rd_kb", uint642char(i->second.num_rd_kb), 8);
        add_assoc_string(pool_stats, "num_wr", uint642char(i->second.num_wr), 9);
        add_assoc_string(pool_stats, "num_wr_kb", uint642char(i->second.num_wr_kb), 10);

        add_assoc_zval(return_value, i->first.c_str(), pool_stats);
    }
}

PHP_METHOD(Rados, get_fs_stats)
{
    statfs_t stats;

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if(rados->get_fs_stats(stats) < 0) {
        RETURN_FALSE;
    }

    array_init(return_value);
    add_assoc_string(return_value, "kb", uint642char(stats.kb), 0);
    add_assoc_string(return_value, "kb_used", uint642char(stats.kb_used), 1);
    add_assoc_string(return_value, "kb_avail", uint642char(stats.kb_avail), 2);
    add_assoc_string(return_value, "num_objects", uint642char(stats.num_objects), 3);
}

PHP_METHOD(Rados, snap_create)
{
    php_rados_pool *pool_r;
    zval *zpool;
    char *snapname=NULL;
    int snapname_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zpool, &snapname, &snapname_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados->snap_create(pool_r->pool, snapname) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, snap_remove)
{
    php_rados_pool *pool_r;
    zval *zpool;
    char *snapname=NULL;
    int snapname_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zpool, &snapname, &snapname_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados->snap_remove(pool_r->pool, snapname) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, selfmanaged_snap_create)
{
    php_rados_pool *pool_r;
    zval *zpool;
    uint64_t snapid;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zpool) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados->selfmanaged_snap_create(pool_r->pool, &snapid) < 0) {
        RETURN_FALSE;
    }

    RETURN_STRINGL(uint642char(snapid), sizeof(snapid), 1);
}

PHP_METHOD(Rados, snap_rollback_object)
{
    php_rados_pool *pool_r;
    zval *zpool;
    char *oid, *snapname=NULL;
    int oid_len, snapname_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &zpool, &oid, &oid_len, &snapname, &snapname_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados->snap_rollback_object(pool_r->pool, oid, snapname) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, list_objects)
{
    Rados::ListCtx ctx;
    php_rados_pool *pool_r;
    zval *zpool;
    int j, r = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zpool) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    array_init(return_value);

    rados->list_objects_open(pool_r->pool, &ctx);
    do {
        std::list<std::string> l;
        r = rados->list_objects_more(ctx, PHP_RADOS_MAX_OBJECTS, l);
        if (r < 0)
            RETURN_NULL();

        for (std::list<std::string>::iterator i = l.begin(); i != l.end(); ++i) {
            add_next_index_string(return_value, i->c_str(), j++);
        }
    } while (r);
    rados->list_objects_close(ctx);
}

PHP_METHOD(Rados, list_objects_open)
{
    Rados::ListCtx ctx;
    php_rados_listctx *listctx_r;
    php_rados_pool *pool_r;
    zval *zpool;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zpool) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    array_init(return_value);

    rados->list_objects_open(pool_r->pool, &ctx);

    listctx_r = (php_rados_listctx *)emalloc(sizeof(php_rados_listctx));
    listctx_r->ctx = ctx;
    ZEND_REGISTER_RESOURCE(return_value, listctx_r, le_rados_listctx);
}

PHP_METHOD(Rados, list_objects_more)
{
    php_rados_listctx *listctx_r;
    zval *zctx;
    int maxobjects;
    int j, r = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zctx, &maxobjects) == FAILURE) {
        RETURN_FALSE;
    }

    if (maxobjects > PHP_RADOS_MAX_OBJECTS) {
        zend_throw_exception(rados_radosexception_ce, "Listing more then PHP_RADOS_MAX_OBJECTS at once is not allowed!", 0 TSRMLS_CC);
        return;
    }

    ZEND_FETCH_RESOURCE(listctx_r, php_rados_listctx*, &zctx, -1, PHP_RADOS_POOL_RES_NAME, le_rados_listctx);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    array_init(return_value);

    do {
        std::list<std::string> l;
        r = rados->list_objects_more(listctx_r->ctx, maxobjects, l);
        if (r < 0)
            RETURN_NULL();

        for (std::list<std::string>::iterator i = l.begin(); i != l.end(); ++i) {
            add_next_index_string(return_value, i->c_str(), j++);
        }
    } while (r);
}

PHP_METHOD(Rados, list_objects_close)
{
    php_rados_listctx *listctx_r;
    zval *zctx;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zctx) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(listctx_r, php_rados_listctx*, &zctx, -1, PHP_RADOS_POOL_RES_NAME, le_rados_listctx);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    rados->list_objects_close(listctx_r->ctx);

}

PHP_METHOD(Rados, create)
{
    php_rados_pool *pool_r;
    char *oid=NULL;
    int oid_len;
    zval *zpool;
    bool exclusive = false;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|b", &zpool, &oid, &oid_len, &exclusive) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    if (rados->create(pool_r->pool, oid, exclusive) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, remove)
{
    php_rados_pool *pool_r;
    char *oid=NULL;
    int oid_len;
    zval *zpool;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zpool, &oid, &oid_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    if (rados->remove(pool_r->pool, oid) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, stat)
{
    php_rados_pool *pool_r;
    char *oid=NULL;
    int oid_len;
    zval *zpool;
    uint64_t size;
    time_t mtime;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zpool, &oid, &oid_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    array_init(return_value);

    if (rados->stat(pool_r->pool, oid, &size, &mtime) < 0) {
        RETURN_FALSE;
    }

    add_assoc_string(return_value, "oid", oid, 1);
    add_assoc_string(return_value, "size", uint642char(size), 2);
    add_assoc_string(return_value, "mtime", uint642char(mtime), 3);
}

PHP_METHOD(Rados, write_full)
{
    php_rados_pool *pool_r;
    char *oid=NULL;
    char *data=NULL;
    int oid_len;
    long data_len;
    zval *zpool;
    bufferlist bl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &zpool, &oid, &oid_len, &data, &data_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    bl.append(data, data_len);

    if (rados->write_full(pool_r->pool, oid, bl) < 0) {
        RETURN_FALSE;
    }
}

PHP_METHOD(Rados, write)
{
    php_rados_pool *pool_r;
    char *oid=NULL;
    char *data=NULL;
    int oid_len;
    long data_len;
    off_t offset = 0;
    zval *zpool;
    bufferlist bl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|l", &zpool, &oid, &oid_len, &data, &data_len, &offset) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    bl.append(data, data_len);

    if (rados->write(pool_r->pool, oid, offset, bl, data_len) < 0) {
        RETURN_FALSE;
    }
}

PHP_METHOD(Rados, read)
{
    php_rados_pool *pool_r;
    char *oid=NULL;
    int oid_len;
    size_t size;
    off_t offset = 0;
    zval *zpool;
    bufferlist bl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl|l", &zpool, &oid, &oid_len, &size, &offset) == FAILURE) {
        RETURN_FALSE;
    }

    if ((int)size < 0) {
        zend_throw_exception(rados_radosexception_ce, "Reading a negative size is not possible!", 0 TSRMLS_CC);
        return;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    if (rados->read(pool_r->pool, oid, offset, bl, size) < 0) {
        RETURN_FALSE;
    }

    RETURN_STRINGL(bl.c_str(), size, 1);
}

PHP_METHOD(Rados, read_full)
{
    php_rados_pool *pool_r;
    char *oid=NULL;
    int oid_len;
    zval *zpool;
    bufferlist bl;
    uint64_t size;
    time_t mtime;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zpool, &oid, &oid_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    if (rados->stat(pool_r->pool, oid, &size, &mtime) < 0) {
        RETURN_FALSE;
    }

    if (rados->read(pool_r->pool, oid, 0, bl, size) < 0) {
        RETURN_FALSE;
    }

    RETURN_STRINGL(bl.c_str(), size, 1);
}

PHP_METHOD(Rados, trunc)
{
    php_rados_pool *pool_r;
    char *oid=NULL;
    int oid_len;
    size_t size;
    zval *zpool;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl", &zpool, &oid, &oid_len, &size) == FAILURE) {
        RETURN_FALSE;
    }

    if ((int)size < 0) {
        zend_throw_exception(rados_radosexception_ce, "Truncating with a negative size is not possible!", 0 TSRMLS_CC);
        return;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    if (rados->trunc(pool_r->pool, oid, size) < 0) {
        RETURN_FALSE;
    }
}

PHP_METHOD(Rados, getxattr)
{
    php_rados_pool *pool_r;
    char *oid, *xattr=NULL;
    int oid_len, xattr_len;
    zval *zpool;
    bufferlist bl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &zpool, &oid, &oid_len, &xattr, &xattr_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    if (rados->getxattr(pool_r->pool, oid, xattr, bl) < 0) {
        RETURN_FALSE;
    }

    RETURN_STRINGL(bl.c_str(), strlen(bl.c_str()), 1);
}

PHP_METHOD(Rados, setxattr)
{
    php_rados_pool *pool_r;
    char *oid, *xattr, *data=NULL;
    int oid_len, xattr_len, data_len;
    zval *zpool;
    bufferlist bl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsss", &zpool, &oid, &oid_len, &xattr, &xattr_len, &data, &data_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    bl.append(data, data_len);

    if (rados->setxattr(pool_r->pool, oid, xattr, bl) < 0) {
        RETURN_FALSE;
    }
}

PHP_METHOD(Rados, getxattrs)
{
    php_rados_pool *pool_r;
    char *oid=NULL;
    int oid_len;
    zval *zpool;
    std::map<std::string, bufferlist> attrset;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zpool, &oid, &oid_len) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(pool_r, php_rados_pool*, &zpool, -1, PHP_RADOS_POOL_RES_NAME, le_rados_pool);

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;

    if (rados->getxattrs(pool_r->pool, oid, attrset) < 0) {
        RETURN_FALSE;
    }

    array_init(return_value);
    int j = 0;
    for(std::map<std::string, bufferlist>::iterator i = attrset.begin(); i != attrset.end(); ++i) {
        add_assoc_string(return_value, i->first.c_str(), i->second.c_str(), j++);
    }

}

PHP_INI_BEGIN()
    PHP_INI_ENTRY("rados.config_file", NULL, PHP_INI_ALL, NULL)
    PHP_INI_ENTRY("rados.monitor_ip", NULL, PHP_INI_ALL, NULL)
    PHP_INI_ENTRY("rados.cephx_keyfile", NULL, PHP_INI_ALL, NULL)
    PHP_INI_ENTRY("rados.cephx_keyring", NULL, PHP_INI_ALL, NULL)
    PHP_INI_ENTRY("rados.cephx_name", NULL, PHP_INI_ALL, NULL)
PHP_INI_END()

PHP_MINIT_FUNCTION(rados)
{
    REGISTER_INI_ENTRIES();

    le_rados_pool = zend_register_list_destructors_ex(NULL, NULL, PHP_RADOS_POOL_RES_NAME, module_number);
    le_rados_listctx = zend_register_list_destructors_ex(NULL, NULL, PHP_RADOS_LISTCTX_RES_NAME, module_number);
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Rados", rados_rados_methods);
    rados_rados_ce = zend_register_internal_class(&ce TSRMLS_CC);
    rados_rados_ce->create_object = rados_create_handler;
    memcpy(&rados_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    rados_object_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "RadosException", php_rados_radosexception_methods);
    rados_radosexception_ce = zend_register_internal_class_ex(&ce, zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);
    rados_radosexception_ce->ce_flags |= ZEND_ACC_FINAL;
    zend_declare_property_long(rados_radosexception_ce, "code", sizeof("code")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(rados)
{
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}

PHP_MINFO_FUNCTION(rados)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "Rados", "enabled");
    php_info_print_table_row(2, "Rados extension version", PHP_RADOS_EXTVER);
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}

zend_module_entry rados_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_RADOS_EXTNAME,
    NULL,                  /* Functions */
    PHP_MINIT(rados),      /* MINIT */
    PHP_MSHUTDOWN(rados),  /* MSHUTDOWN */
    NULL,                  /* RINIT */
    NULL,                  /* RSHUTDOWN */
    PHP_MINFO(rados),      /* MINFO */
    PHP_RADOS_EXTVER,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_RADOS
extern "C" {
    ZEND_GET_MODULE(rados)
}
#endif
