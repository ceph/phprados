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

ZEND_BEGIN_ARG_INFO(arginfo_rados_create, 0)
    ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

const zend_function_entry rados_functions[] = {
    PHP_FE(rados_create, arginfo_rados_create)
    {NULL, NULL, NULL}
};


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

    if (id == NULL) {
        id = "someid";
    }

    if (rados_create(&cluster, id) < 0) {
        RETURN_FALSE;
    }

    cluster_r = (php_rados_cluster *)emalloc(sizeof(php_rados_cluster));
    cluster_r->cluster = cluster;
    ZEND_REGISTER_RESOURCE(return_value, cluster_r, le_rados_cluster);
}

PHP_FUNCTION(rados_shutdown)
{
    php_rados_cluster *cluster_r;
    zval *zpool;

    ZEND_FETCH_RESOURCE(cluster_r, php_rados_cluster*, &zpool, -1, PHP_RADOS_CLUSTER_RES_NAME, le_rados_cluster);

    rados_shutdown(cluster_r->cluster);

    RETURN_TRUE;
}

PHP_MINIT_FUNCTION(rados)
{
    le_rados_cluster = zend_register_list_destructors_ex(NULL, NULL, PHP_RADOS_CLUSTER_RES_NAME, module_number);

    return SUCCESS;
}

PHP_MINFO_FUNCTION(rados)
{
    int major, minor, extra = 0;
	char compiled_ver[16], linked_ver[16];

    rados_version(&major, &minor, &extra);

	sprintf(linked_ver, "%d.%d.%d", LIBRADOS_VER_MAJOR, LIBRADOS_VER_MINOR, LIBRADOS_VER_EXTRA);
	sprintf(compiled_ver, "%d.%d.%d", major, minor, extra);

    php_info_print_table_start();
    php_info_print_table_row(2, "Rados", "enabled");
    php_info_print_table_row(2, "Rados extension version", PHP_RADOS_EXTVER);
    php_info_print_table_row(2, "librados version (linked)", linked_ver);
    php_info_print_table_row(2, "librados version (compiled)", compiled_ver);
    php_info_print_table_end();
}

zend_module_entry rados_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_RADOS_EXTNAME,
    rados_functions,       /* Functions */
    PHP_MINIT(rados),      /* MINIT */
    NULL,                  /* MSHUTDOWN */
    NULL,                  /* RINIT */
    NULL,                  /* RSHUTDOWN */
    PHP_MINFO(rados),      /* MINFO */
    PHP_RADOS_EXTVER,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_RADOS
ZEND_GET_MODULE(rados)
#endif
