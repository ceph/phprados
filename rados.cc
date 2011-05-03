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
#include <sstream>
#include <vector>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_rados.h"
#include "ioctx.h"

zend_object_handlers rados_object_handlers;
zend_object_handlers rados_ioctx_object_handlers;

zend_class_entry *rados_rados_ce;
zend_class_entry *rados_radosexception_ce;
zend_class_entry *rados_radosioctx_ce;

ZEND_BEGIN_ARG_INFO(arginfo_rados___construct, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_init, 0)
    ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_connect, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_conf_read_file, 0)
    ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_conf_set, 0)
    ZEND_ARG_INFO(0, option)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_conf_get, 0)
    ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_shutdown, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_pool_create, 0)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, auid)
    ZEND_ARG_INFO(0, crushrule)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_pool_lookup, 0)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_pool_delete, 0)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_pool_list, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_get_pool_stats, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_cluster_stat, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_ioctx_create, 0)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

const zend_function_entry rados_rados_methods[] = {
    PHP_ME(Rados, __construct, arginfo_rados___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Rados, init, arginfo_rados_init, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, connect, arginfo_rados_connect, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, conf_read_file, arginfo_rados_conf_read_file, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, conf_set, arginfo_rados_conf_set, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, conf_get, arginfo_rados_conf_get, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, shutdown, arginfo_rados_shutdown, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, pool_create, arginfo_rados_pool_create, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, pool_lookup, arginfo_rados_pool_lookup, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, pool_delete, arginfo_rados_pool_delete, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, pool_list, arginfo_rados_pool_list, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, get_pool_stats, arginfo_rados_get_pool_stats, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, cluster_stat, arginfo_rados_cluster_stat, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, ioctx_create, arginfo_rados_ioctx_create, ZEND_ACC_PUBLIC)
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

namespace {

    char *uint642char(uint64_t u)
    {
        std::stringstream c_s;
        c_s << u;
        char *c_buf = (char *)emalloc(sizeof(c_s.str().c_str()));
        strcpy(c_buf, c_s.str().c_str());
        return c_buf;
    }

    const char *cp_zval_strval(zval &z)
    {
        std::stringstream ss(Z_STRVAL(z));
        char *buf = (char *)emalloc(ss.str().size() + 1);
        strcpy(buf, ss.str().c_str());
        return (const char *)buf;
    }
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

    retval.handle = zend_objects_store_put(obj, NULL, rados_free_storage, NULL TSRMLS_CC);
    retval.handlers = &rados_object_handlers;

    return retval;
}

PHP_METHOD(Rados, __construct)
{
    Rados *rados = NULL;
    zval *object = getThis(), *options = NULL;

    rados = new Rados();
    rados_object *obj = (rados_object *)zend_object_store_get_object(object TSRMLS_CC);
    obj->rados = rados;
}

PHP_METHOD(Rados, init)
{
    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    if (!obj->initialized) {
        rados = obj->rados;
        if (rados->init(NULL) < 0) {
            zend_throw_exception(rados_radosexception_ce, "Failed to initialize RADOS!", 0 TSRMLS_CC);
            return;
        }
        obj->initialized = true;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, connect)
{
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->rados->connect() < 0) {
        zend_throw_exception(rados_radosexception_ce, "Failed to connect to the RADOS cluster", 0 TSRMLS_CC);
        return;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, conf_read_file)
{

    char *path = NULL;
    int path_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE) {
        RETURN_NULL();
    }

    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->rados->conf_read_file(path) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, conf_set)
{

    char *option = NULL;
    char *value = NULL;
    int option_len = 0;
    int value_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &option, &option_len, &value, &value_len) == FAILURE) {
        RETURN_NULL();
    }

    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->rados->conf_set(option, value) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, conf_get)
{

    char *option = NULL;
    int option_len = 0;
    std::string rvalue;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &option, &option_len) == FAILURE) {
        RETURN_NULL();
    }

    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->rados->conf_get(option, rvalue) < 0) {
        RETURN_FALSE;
    }

    RETURN_STRINGL(rvalue.c_str(), rvalue.length(), 1);
}

PHP_METHOD(Rados, shutdown)
{
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    obj->rados->shutdown();

    RETURN_TRUE;
}

PHP_METHOD(Rados, pool_create)
{
    char *name = NULL;
    int name_len = 0;
    long auid, crushrule = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ll", &name, &name_len, &auid, &crushrule) == FAILURE) {
        RETURN_NULL();
    }

    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (auid == NULL || crushrule == NULL) {
        if (obj->rados->pool_create(name) < 0) {
            RETURN_FALSE;
        }
    } else {
        if (obj->rados->pool_create(name, auid, crushrule) < 0) {
            RETURN_FALSE;
        }
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, pool_lookup)
{
    char *name = NULL;
    int name_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
        RETURN_NULL();
    }

    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->rados->pool_lookup(name) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, pool_delete)
{
    char *name = NULL;
    int name_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
        RETURN_NULL();
    }

    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->rados->pool_delete(name) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(Rados, pool_list)
{
    std::list<std::string> pools;

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if(obj->rados->pool_list(pools) < 0) {
        RETURN_FALSE;
    }

    array_init(return_value);
    for (std::list<std::string>::iterator i = pools.begin(); i != pools.end(); i++) {
        add_next_index_string(return_value, i->c_str(), 1);
    }
}

PHP_METHOD(Rados, get_pool_stats)
{
    std::list<std::string> v;
    std::map<std::string,pool_stat_t> stats;
    zval *pools;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &pools) == FAILURE) {
        RETURN_NULL();
    }

    if (Z_TYPE_P(pools) == IS_STRING) {
        v.push_back(Z_STRVAL_P(pools));
    } else if (Z_TYPE_P(pools) == IS_ARRAY) {
        HashTable *arr_hash;
        HashPosition hash_pos;
        zval **arr_value;
        arr_hash = Z_ARRVAL_P(pools);

        for (zend_hash_internal_pointer_reset_ex(arr_hash, &hash_pos); zend_hash_get_current_data_ex(arr_hash, (void**) &arr_value, &hash_pos) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &hash_pos)) {
            if (Z_TYPE_PP(arr_value) == IS_STRING) {
                v.push_back(Z_STRVAL_PP(arr_value));
            }
        }
    } else {
        RETURN_FALSE;
    }

    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if(obj->rados->get_pool_stats(v, stats) < 0) {
        RETURN_FALSE;
    }

    array_init(return_value);
    for (std::map<std::string,pool_stat_t>::iterator i = stats.begin(); i != stats.end(); i++) {
        zval *pool_stats;
        ALLOC_INIT_ZVAL(pool_stats);
        array_init(pool_stats);

        add_assoc_string(pool_stats, "num_bytes", uint642char(i->second.num_bytes), 1);
        add_assoc_string(pool_stats, "num_kb", uint642char(i->second.num_kb), 1);
        add_assoc_string(pool_stats, "num_objects", uint642char(i->second.num_objects), 1);
        add_assoc_string(pool_stats, "num_object_clones", uint642char(i->second.num_object_clones), 1);
        add_assoc_string(pool_stats, "num_object_copies", uint642char(i->second.num_object_copies), 1);
        add_assoc_string(pool_stats, "num_objects_missing_on_primary", uint642char(i->second.num_objects_missing_on_primary), 1);
        add_assoc_string(pool_stats, "num_objects_degraded", uint642char(i->second.num_objects_degraded), 1);
        add_assoc_string(pool_stats, "num_rd", uint642char(i->second.num_rd), 1);
        add_assoc_string(pool_stats, "num_rd_kb", uint642char(i->second.num_rd_kb), 1);
        add_assoc_string(pool_stats, "num_wr", uint642char(i->second.num_wr), 1);
        add_assoc_string(pool_stats, "num_wr_kb", uint642char(i->second.num_wr_kb), 1);

        add_assoc_zval(return_value, i->first.c_str(), pool_stats);
    }
}

PHP_METHOD(Rados, cluster_stat)
{
    cluster_stat_t stats;

    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if(obj->rados->cluster_stat(stats) < 0) {
        RETURN_FALSE;
    }

    array_init(return_value);
    add_assoc_string(return_value, "kb", uint642char(stats.kb), 1);
    add_assoc_string(return_value, "kb_used", uint642char(stats.kb_used), 1);
    add_assoc_string(return_value, "kb_avail", uint642char(stats.kb_avail), 1);
    add_assoc_string(return_value, "num_objects", uint642char(stats.num_objects), 1);
}

PHP_METHOD(Rados, ioctx_create)
{
    char *name = NULL;
    int name_len = 0;
    IoCtx pioctx;
    struct radosioctx_object *riob;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
        RETURN_NULL();
    }

    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->rados->ioctx_create(name, pioctx) < 0) {
        RETURN_FALSE;
    }

    if (object_init_ex(return_value, rados_radosioctx_ce) != SUCCESS) {
        RETURN_FALSE;
    }

    riob = (struct radosioctx_object *) zend_object_store_get_object(return_value TSRMLS_CC);
    riob->ioctx = &pioctx;
}

PHP_MINIT_FUNCTION(rados)
{
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

    INIT_CLASS_ENTRY(ce, "RadosIoCtx", php_rados_ioctx_methods);
    rados_radosioctx_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
    rados_radosioctx_ce->ce_flags |= ZEND_ACC_FINAL;
    memcpy(&rados_ioctx_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    rados_ioctx_object_handlers.clone_obj = NULL;

    return SUCCESS;
}

PHP_MINFO_FUNCTION(rados)
{
    int major, minor, extra = 0;
    std::stringstream librados_version, compiled_version;

    Rados rados;
    rados.version(&major, &minor, &extra);

    librados_version << major << "." << minor << "." << extra;
    compiled_version << LIBRADOS_VER_MAJOR << "." << LIBRADOS_VER_MINOR << "." << LIBRADOS_VER_EXTRA;

    php_info_print_table_start();
    php_info_print_table_row(2, "Rados", "enabled");
    php_info_print_table_row(2, "Rados extension version", PHP_RADOS_EXTVER);
    php_info_print_table_row(2, "librados version (linked)", librados_version.str().c_str());
    php_info_print_table_row(2, "librados version (compiled)", compiled_version.str().c_str());
    php_info_print_table_end();
}

zend_module_entry rados_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_RADOS_EXTNAME,
    NULL,                  /* Functions */
    PHP_MINIT(rados),      /* MINIT */
    NULL,                  /* MSHUTDOWN */
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
