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

ZEND_BEGIN_ARG_INFO(arginfo_radosioctx_set_auid, 0)
    ZEND_ARG_INFO(0, auid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_radosioctx_create, 0)
    ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

const zend_function_entry php_rados_ioctx_methods[] = {
    PHP_ME(RadosIoCtx, set_auid, arginfo_radosioctx_set_auid, ZEND_ACC_PUBLIC)
    PHP_ME(RadosIoCtx, create, arginfo_radosioctx_create, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

PHP_METHOD(RadosIoCtx, set_auid)
{
    long auid;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &auid) == FAILURE) {
        RETURN_NULL();
    }

    radosioctx_object *obj = (radosioctx_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->ioctx->set_auid(auid) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(RadosIoCtx, create)
{
    char *object = NULL;
    int object_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &object, &object_len) == FAILURE) {
        RETURN_NULL();
    }

    radosioctx_object *obj = (radosioctx_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (obj->ioctx->create(object, false) < 0) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}