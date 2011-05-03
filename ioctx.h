struct radosioctx_object
{
    zend_object std;
    IoCtx *ioctx;
};

PHP_METHOD(RadosIoCtx, set_auid);
PHP_METHOD(RadosIoCtx, create);