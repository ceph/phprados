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

#include "php_librados.h"

zend_object_handlers rados_object_handlers;
zend_class_entry *rados_ce;

int le_rados_pool;

struct rados_object {
    zend_object std;
    Rados *rados;
};

function_entry rados_methods[] = {
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
    PHP_ME(Rados, list_objects, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, create, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, remove, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, stat, NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void rados_free_storage(void *object TSRMLS_DC)
{
    Rados *rados;
    rados_object *obj = (rados_object *)object;
    rados = obj->rados;
    if (rados != NULL) {
        rados->shutdown();
    }
    delete obj->rados;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);

    efree(obj);
}

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

    retval.handle = zend_objects_store_put(obj, NULL,
    rados_free_storage, NULL TSRMLS_CC);
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
    int argc = 1;
    const char *argv[0];
    Rados *rados;

    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados->initialize(argc, argv) < 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to initialize RADOS!");
        RETURN_NULL();
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
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "The specified RADOS poolname (%s) is too long!", spool);
    }

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if(rados->open_pool(spool, &pool) < 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "The specified RADOS pool (%s) could not be opened.", spool);
        RETURN_NULL();
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
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "The specified RADOS poolname (%s) is too long!", spool);
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
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "The specified RADOS poolname (%s) is too long!", spool);
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
    std::list<string> pools;
    std::list<string>::iterator i;

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if(rados->list_pools(pools) < 0) {
        RETURN_FALSE;
    }

    array_init(return_value);
    int j = 0;
    for (i = pools.begin(); i != pools.end(); i++) {
        add_next_index_string(return_value, i->c_str(), j);
        j++;
    }
}

PHP_METHOD(Rados, snap_create)
{
    php_rados_pool *pool_r;
    zval *zpool;
    char *snapname=NULL;
    int snapname_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sr", &snapname, &snapname_len, &zpool) == FAILURE) {
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

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sr", &snapname, &snapname_len, &zpool) == FAILURE) {
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

PHP_METHOD(Rados, list_objects)
{
    Rados::ListCtx ctx;
    php_rados_pool *pool_r;
    zval *zpool;
    list<string>::iterator i;
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
        list<string> l;
        r = rados->list_objects_more(ctx, PHP_RADOS_MAX_OBJECTS, l);
        if (r < 0)
            RETURN_NULL();

        for (i = l.begin(); i != l.end(); ++i) {
            add_next_index_string(return_value, i->c_str(), j++);
        }
    } while (r);
    rados->list_objects_close(ctx);
}

PHP_METHOD(Rados, create)
{
    php_rados_pool *pool_r;
    char *oid=NULL;
    int oid_len;
    zval *zpool;
    bool exclusive = false;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sr|b", &oid, &oid_len, &zpool, &exclusive) == FAILURE) {
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

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sr", &oid, &oid_len, &zpool) == FAILURE) {
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
    size_t size;
    time_t mtime;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sr", &oid, &oid_len, &zpool) == FAILURE) {
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

    stringstream size_s;
    size_s << size;
    char *size_buf = (char *)emalloc(sizeof(size_s.str().c_str()));
    strcpy(size_buf, size_s.str().c_str());

    stringstream mtime_s;
    mtime_s << mtime;
    char *mtime_buf = (char *)emalloc(sizeof(mtime_s.str().c_str()));
    strcpy(mtime_buf, mtime_s.str().c_str());

    add_assoc_string(return_value, "oid", oid, 1);
    add_assoc_string(return_value, "size", size_buf, 2);
    add_assoc_string(return_value, "mtime", mtime_buf, 3);
}

PHP_MINIT_FUNCTION(librados)
{
    le_rados_pool = zend_register_list_destructors_ex(NULL, NULL, PHP_RADOS_POOL_RES_NAME, module_number);
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Rados", rados_methods);
    rados_ce = zend_register_internal_class(&ce TSRMLS_CC);
    rados_ce->create_object = rados_create_handler;
    memcpy(&rados_object_handlers,zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    rados_object_handlers.clone_obj = NULL;
    return SUCCESS;
}

zend_module_entry librados_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_LIBRADOS_EXTNAME,
    NULL,                  /* Functions */
    PHP_MINIT(librados),
    NULL,                  /* MSHUTDOWN */
    NULL,                  /* RINIT */
    NULL,                  /* RSHUTDOWN */
    NULL,                  /* MINFO */
    PHP_LIBRADOS_EXTVER,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_LIBRADOS
extern "C" {
    ZEND_GET_MODULE(librados)
}
#endif
