#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_rados.h"

#include "rados_interfaces.h"

zend_class_entry *rados_radosproxy_ce;
zend_class_entry *rados_radosobject_ce;
zend_class_entry *rados_radospool_ce;

ZEND_BEGIN_ARG_INFO(arginfo_rados_void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_radosproxy_setrados, 0)
    ZEND_ARG_INFO(1, "rados")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rados_radosproxy_call, 0, 0, 1)
    ZEND_ARG_INFO(0, "method")
    ZEND_ARG_ARRAY_INFO(0, "args", 1)
ZEND_END_ARG_INFO()

const zend_function_entry php_rados_radosproxy_methods[] = {
    PHP_ABSTRACT_ME(RadosProxy, getRados, arginfo_rados_void)
    PHP_ABSTRACT_ME(RadosProxy, setRados, arginfo_rados_radosproxy_setrados)
    PHP_ABSTRACT_ME(RadosProxy, call, arginfo_rados_radosproxy_call)
    {NULL, NULL, NULL}
};

const zend_function_entry php_rados_radosobject_methods[] = {
    PHP_ABSTRACT_ME(RadosObject, getOid, arginfo_rados_void)
    PHP_ABSTRACT_ME(RadosObject, getSize, arginfo_rados_void)
    {NULL, NULL, NULL}
};

const zend_function_entry php_rados_radospool_methods[] = {
    PHP_ABSTRACT_ME(RadosPool, getRadosProxy, arginfo_rados_void)
    PHP_ABSTRACT_ME(RadosPool, getResource, arginfo_rados_void)
    {NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(rados_interfaces)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "RadosProxy", php_rados_radosproxy_methods);
    rados_radosproxy_ce = zend_register_internal_interface(&ce TSRMLS_CC);

    INIT_CLASS_ENTRY(ce, "RadosObject", php_rados_radosobject_methods);
    rados_radosobject_ce = zend_register_internal_interface(&ce TSRMLS_CC);

    INIT_CLASS_ENTRY(ce, "RadosPool", php_rados_radospool_methods);
    rados_radospool_ce = zend_register_internal_interface(&ce TSRMLS_CC);

    return SUCCESS;
}
