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

zend_object_handlers rados_object_handlers;

zend_class_entry *rados_rados_ce;
zend_class_entry *rados_radosexception_ce;

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

const zend_function_entry rados_rados_methods[] = {
    PHP_ME(Rados, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Rados, init, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, connect, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, conf_read_file, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, conf_set, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, conf_get, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, shutdown, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, pool_create, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, pool_lookup, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados, pool_delete, NULL, ZEND_ACC_PUBLIC)
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
    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    rados = obj->rados;
    if (rados->connect() < 0) {
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

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    rados = obj->rados;
    if (rados->conf_read_file(path) < 0) {
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

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    rados = obj->rados;
    if (rados->conf_set(option, value) < 0) {
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

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    rados = obj->rados;
    if (rados->conf_get(option, rvalue) < 0) {
        RETURN_FALSE;
    }

    RETURN_STRINGL(rvalue.c_str(), rvalue.length(), 1);
}

PHP_METHOD(Rados, shutdown)
{
    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    rados = obj->rados;
    rados->shutdown();

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

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    rados = obj->rados;

    if (auid == NULL || crushrule == NULL) {
        if (rados->pool_create(name) < 0) {
            RETURN_FALSE;
        }
    } else {
        if (rados->pool_create(name, auid, crushrule) < 0) {
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

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    rados = obj->rados;
    if (rados->pool_lookup(name) < 0) {
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

    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    rados = obj->rados;
    if (rados->pool_delete(name) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
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
