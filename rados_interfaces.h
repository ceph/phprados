#ifndef RADOS_INTERFACES_H
#define RADOS_INTERFACES_H

extern "C" {
#include "php.h"
#include "php_rados.h"
}

extern zend_class_entry *rados_radosproxy_ce;
extern zend_class_entry *rados_radosobject_ce;
extern zend_class_entry *rados_radospool_ce;

PHP_MINIT_FUNCTION(rados_interfaces);

#endif
