#include "php_librados.h"

using namespace std;

#include "rados/librados.hpp"

using namespace librados;

zend_object_handlers rados_object_handlers;
zend_class_entry *rados_ce;

struct rados_object {
	zend_object std;
	Rados *rados;
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_rados_open_pool, 0, 0, 1)
    ZEND_ARG_INFO(0, pool)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rados_close_pool, 0, 0, 1)
    ZEND_ARG_INFO(0, pool)
ZEND_END_ARG_INFO()

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
    if (rados != NULL) {
        if (rados->initialize(argc, argv) < 0) {
            RETURN_NULL();
        }
    }
}

PHP_METHOD(Rados, open_pool)
{
    pool_t pool;
    char *spool;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &spool) == FAILURE) {
        RETURN_NULL();
    }
    
    Rados *rados;
    rados_object *obj = (rados_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    rados = obj->rados;
    if (rados != NULL) {
        if(rados->open_pool(spool, &pool) < 0) {
            RETURN_NULL();
        }
    }

}

PHP_METHOD(Rados, close_pool)
{
}

PHP_METHOD(Rados, lookup_pool)
{
}

PHP_METHOD(Rados, create)
{
}

function_entry rados_methods[] = {
	PHP_ME(Rados, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Rados, initialize, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Rados, open_pool, arginfo_rados_open_pool, ZEND_ACC_PUBLIC)
	PHP_ME(Rados, close_pool, arginfo_rados_close_pool, ZEND_ACC_PUBLIC)
	PHP_ME(Rados, lookup_pool, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Rados, create, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(librados)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Rados", rados_methods);
	rados_ce = zend_register_internal_class(&ce TSRMLS_CC);
	rados_ce->create_object = rados_create_handler;
	memcpy(&rados_object_handlers,zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	rados_object_handlers.clone_obj = NULL;
	return SUCCESS;
}

zend_module_entry librados_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_LIBRADOS_EXTNAME,
    NULL,                  /* Functions */
    PHP_MINIT(librados),
    NULL,                  /* MSHUTDOWN */
    NULL,                  /* RINIT */
    NULL,                  /* RSHUTDOWN */
    NULL,                  /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
    PHP_LIBRADOS_EXTVER,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_LIBRADOS
extern "C" {
    ZEND_GET_MODULE(librados)
}
#endif
