/*
 * phprados - A PHP5 extension for using librados
 *
 * Copyright (C) 2011 Wido den Hollander <wido@widodh.nl>
 * Copyright (C) 2013 Jeroen Moors <jeroen.moors@fluoline.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */
 
/**
 * This file serves as an entry point for the extension.
 * It includes the procedural and object-oriented code as if enabled
 * with --enable-oo
 */
// Definitions shared by both interfaces
#include "config.h"
#include "php_rados.h"

// Definitions for the procedural interface
#ifdef BUILD_PROCEDURAL
	#include "php_rados_pr.h"
	#include "php_rados_pr.c"
#endif

// Definitions for the OO interface
#ifdef BUILD_OO
	#include "php_rados_oo.h"
	#include "php_rados_oo.cc"
#endif

PHP_MINIT_FUNCTION(rados)
{
	#ifdef BUILD_PROCEDURAL
		le_rados_cluster = zend_register_list_destructors_ex(NULL, NULL, PHP_RADOS_CLUSTER_RES_NAME, module_number);
		le_rados_ioctx = zend_register_list_destructors_ex(NULL, NULL, PHP_RADOS_IOCTX_RES_NAME, module_number);
	#endif
	
	#ifdef BUILD_OO
		zend_class_entry ce;
		INIT_CLASS_ENTRY(ce, "Rados", rados_methods);
		rados_ce = zend_register_internal_class(&ce TSRMLS_CC);
		rados_ce->create_object = rados_create_handler;
		memcpy(&rados_object_handlers,
			zend_get_std_object_handlers(), sizeof(zend_object_handlers));
		rados_object_handlers.clone_obj = NULL;
	#endif
	
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(rados)
{
    return SUCCESS;
}

PHP_MINFO_FUNCTION(rados)
{
    int major, minor, extra = 0;
    char compiled_ver[16], linked_ver[16], output_buf[8];

    rados_version(&major, &minor, &extra);

    sprintf(linked_ver, "%d.%d.%d", LIBRADOS_VER_MAJOR, LIBRADOS_VER_MINOR, LIBRADOS_VER_EXTRA);
    sprintf(compiled_ver, "%d.%d.%d", major, minor, extra);

    php_info_print_table_start();
    php_info_print_table_row(2, "Rados", "enabled");
    php_info_print_table_row(2, "Rados extension version", PHP_RADOS_EXTVER);
    php_info_print_table_row(2, "librados version (linked)", linked_ver);
    php_info_print_table_row(2, "librados version (compiled)", compiled_ver);

	#ifdef BUILD_PROCEDURAL
		php_info_print_table_row(2, "Procedural interface", "enabled");
	#else
		php_info_print_table_row(2, "Procedural interface", "disabled");
	#endif

	#ifdef BUILD_OO
		php_info_print_table_row(2, "Object oriented interface", "enabled");
	#else
		php_info_print_table_row(2, "Object oriented interface", "disabled");
	#endif
	
    sprintf(output_buf, "%d", PHP_RADOS_SNAP_NAME_MAX_LENGTH);
    php_info_print_table_row(2, "Maximum length snapshot name", output_buf);

    sprintf(output_buf, "%d", PHP_RADOS_SNAP_MAX_NUM);
    php_info_print_table_row(2, "Maximum snapshots per pool", output_buf);
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}

zend_module_entry rados_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_RADOS_EXTNAME,
    #ifdef BUILD_PROCEDURAL
		rados_functions,	/* Functions */
	#else	
		NULL,				/* No functions */
	#endif
    PHP_MINIT(rados),       /* MINIT */
    PHP_MSHUTDOWN(rados),	/* MSHUTDOWN */
    NULL,					/* RINIT */
    NULL,					/* RSHUTDOWN */
    PHP_MINFO(rados),		/* MINFO */
    PHP_RADOS_EXTVER,
    STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_RADOS
ZEND_GET_MODULE(rados)
#endif
