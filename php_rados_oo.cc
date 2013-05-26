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

#include "php_rados_oo.h"
#include "rados_oo.h"

#include "php.h"
#include "php_ini.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"

zend_object_handlers rados_object_handlers;

struct rados_object {
    zend_object std;
    Rados *rados;
};

zend_class_entry *rados_ce;

void rados_free_storage(void *object TSRMLS_DC) {
    rados_object *obj = (rados_object *)object;
    delete obj->rados; 

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);

    efree(obj);
}

zend_object_value rados_create_handler(zend_class_entry *type TSRMLS_DC) {
    zval *tmp;
    zend_object_value retval;

    rados_object *obj = (rados_object *)emalloc(sizeof(rados_object));
    memset(obj, 0, sizeof(rados_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(obj->std.properties, &type->default_properties,
        (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));

    retval.handle = zend_objects_store_put(obj, NULL, rados_free_storage, NULL TSRMLS_CC);
    retval.handlers = &rados_object_handlers;

    return retval;
}

PHP_METHOD(Rados, __construct) {
	char *configname;
	int configname_len;
	char *poolname;
	int poolname_len;
	
    Rados *rados = NULL;
    zval *object = getThis();
    rados = new Rados();
    rados_object *obj = (rados_object *)zend_object_store_get_object(object TSRMLS_CC);
    obj->rados = rados;
}

PHP_METHOD(Rados, __destruct) {
	Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados != NULL) {
         delete rados;
    }
}

PHP_METHOD(Rados, setOption) {
	Rados *rados;
	char *option = NULL;
	int option_len = 0;

	char *value = NULL;
	int value_len = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",  &option, &option_len, &value, &value_len) == FAILURE) {
        RETURN_NULL();
    }

    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados != NULL) {
		RETURN_BOOL(rados->setOption(option, value));
    }
}

PHP_METHOD(Rados, getOption) {
	Rados *rados;
	char *option = NULL;
	int option_len = 0;

	char value[256];
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",  &option, &option_len) == FAILURE) {
        RETURN_NULL();
    }

    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados != NULL) {
		if(rados->getOption(option, value)) {
			RETURN_STRINGL(value, strlen(value), 1);
		} else {
			RETURN_BOOL(0);
		}
    }
}



PHP_METHOD(Rados, connect) {
	Rados *rados;
	int err;
	char *mon_host = NULL;
	int mon_host_len = 0;
	char *key = NULL;
	int key_len = 0;
	char *poolname = NULL;
	int poolname_len = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sss",  &mon_host, &mon_host_len, &key, &key_len, &poolname, &poolname_len) == FAILURE) {
        RETURN_NULL();
    }
    
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados != NULL) {
		if (mon_host != NULL) {
			err = rados->setOption("mon_host", mon_host);
			if (!err) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, rados->lastErrorMessage);
				RETURN_BOOL(false);
			}
		}

		if (key != NULL) {
			err = rados->setOption("key", key);
			if (!err) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, rados->lastErrorMessage);
				RETURN_BOOL(false);
			}
		}
		
		err = rados->connect();
		if (!err) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, rados->lastErrorMessage);
			RETURN_BOOL(false);
		}
		
		if (poolname != NULL) {
			err = rados->selectPool(poolname);
			if (!err) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, rados->lastErrorMessage);
				RETURN_BOOL(false);
			}
		}
		RETURN_BOOL(true);
    }
}

PHP_METHOD(Rados, readConfig) {
	int err;
	char *filename;
	int filename_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",  &filename, &filename_len) == FAILURE) {
        RETURN_NULL();
    }
    
	Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    
    if (rados != NULL) {
		err = rados->readConfig(filename);
		if (!err) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, rados->lastErrorMessage);
			RETURN_BOOL(false);
		} else {
			RETURN_BOOL(true);
		}
    }
}

PHP_METHOD(Rados, createPool) {
	int err;
	Rados *rados;
	char *poolname;
	int poolname_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",  &poolname, &poolname_len) == FAILURE) {
        RETURN_NULL();
    }
    
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados != NULL) {
        err = rados->createPool(poolname);
   		if (!err) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, rados->lastErrorMessage);
			RETURN_BOOL(false);
		} else {
			RETURN_BOOL(true);
		}
    }
}

PHP_METHOD(Rados, destroyPool) {
	Rados *rados;
	char *poolname;
	int poolname_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",  &poolname, &poolname_len) == FAILURE) {
        RETURN_NULL();
    }
    
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados != NULL) {
        RETURN_BOOL(rados->destroyPool(poolname));
    }
}

PHP_METHOD(Rados, selectPool) {
	Rados *rados;
	char *poolname;
	int poolname_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",  &poolname, &poolname_len) == FAILURE) {
        RETURN_NULL();
    }
    
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados != NULL) {
        RETURN_BOOL(rados->selectPool(poolname));
    }
}



PHP_METHOD(Rados, write) {
	char *key;
	int key_len;

	char *value;
	size_t value_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l",  &key, &key_len, &value, &value_len) == FAILURE) {
        RETURN_NULL();
    }
    
	Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    
    if (rados != NULL) {
        RETURN_BOOL(rados->write(key, value, value_len));
        //rados->write(key, value, value_len);
        fprintf(stderr, "Going back to PHP\n");
    }
}


PHP_METHOD(Rados, read) {
	char *key;
	int key_len;
	size_t buffer_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",  &key, &key_len) == FAILURE) {
        RETURN_NULL();
    }
    
	Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    
    if (rados != NULL) {
		buffer_len = rados->getSize(key);
		char buffer[buffer_len];
		rados->read(key, buffer, buffer_len);
		RETURN_STRINGL(buffer, buffer_len, 1);
    }
}

PHP_METHOD(Rados, getLastError) {    
	Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    
    if (rados != NULL) {
		RETURN_STRINGL(rados->lastErrorMessage, strlen(rados->lastErrorMessage), 1);
	}
}


function_entry rados_methods[] = {
    PHP_ME(Rados,  	__construct,	NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Rados,  	connect,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados,	setOption,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Rados,	getOption,		NULL, ZEND_ACC_PUBLIC)    
    PHP_ME(Rados,	readConfig,		NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Rados,	createPool,		NULL, ZEND_ACC_PUBLIC) 
	PHP_ME(Rados,	selectPool,		NULL, ZEND_ACC_PUBLIC) 
	PHP_ME(Rados,	destroyPool,	NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados,	write,			NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Rados,	read,			NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Rados,	getLastError,	NULL, ZEND_ACC_PUBLIC)    
    {NULL, NULL, NULL}
};
