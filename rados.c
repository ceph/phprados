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

ZEND_BEGIN_ARG_INFO(arginfo_rados_create, 0)
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

ZEND_BEGIN_ARG_INFO(arginfo_rados_ioctx_destroy, 0)
	ZEND_ARG_INFO(0, ioctx)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_pool_lookup, 0)
	ZEND_ARG_INFO(0, cluster)
	ZEND_ARG_INFO(0, pool)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rados_pool_create, 0)
	ZEND_ARG_INFO(0, cluster)
	ZEND_ARG_INFO(0, pool)
	ZEND_ARG_INFO(0, options)
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
	PHP_FE(rados_pool_lookup, arginfo_rados_pool_lookup)
	PHP_FE(rados_pool_create, arginfo_rados_pool_create)
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
    zval *zcluster;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zcluster) == FAILURE) {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(cluster_r, php_rados_cluster*, &zcluster, -1, PHP_RADOS_CLUSTER_RES_NAME, le_rados_cluster);

    rados_shutdown(cluster_r->cluster);

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
	long option;
	HashPosition pos;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|a", &zcluster, &pool, &pool_len, &options) == FAILURE) {
        RETURN_NULL();
    }
    
    if (Z_TYPE_P(options) == IS_ARRAY) {
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

PHP_MINIT_FUNCTION(rados)
{
	le_rados_cluster = zend_register_list_destructors_ex(NULL, NULL, PHP_RADOS_CLUSTER_RES_NAME, module_number);
	le_rados_ioctx = zend_register_list_destructors_ex(NULL, NULL, PHP_RADOS_IOCTX_RES_NAME, module_number);

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
