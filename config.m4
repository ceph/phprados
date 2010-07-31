PHP_ARG_ENABLE(librados,
    [Enable the librados extension],
    [  --enable-librados[=DIR]      Enable "librados" extension support])

if test $PHP_LIBRADOS != "no"; then
    PHP_REQUIRE_CXX()
    PHP_SUBST(LIBRADOS_SHARED_LIBADD)
    PHP_ADD_LIBRARY(rados,, LIBRADOS_SHARED_LIBADD)
    PHP_ADD_LIBRARY(crypto,, LIBRADOS_SHARED_LIBADD)
    PHP_NEW_EXTENSION(librados, librados.cc, $ext_shared)
fi