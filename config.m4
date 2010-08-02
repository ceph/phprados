PHP_ARG_ENABLE(rados,
    [Enable the rados extension],
    [  --enable-rados[=DIR]      Enable "RADOS" extension support])

if test $PHP_RADOS != "no"; then
    PHP_REQUIRE_CXX()
    PHP_SUBST(RADOS_SHARED_LIBADD)
    PHP_ADD_LIBRARY(rados,, RADOS_SHARED_LIBADD)
    PHP_ADD_LIBRARY(crypto,, RADOS_SHARED_LIBADD)
    PHP_NEW_EXTENSION(rados, rados.cc, $ext_shared)
fi