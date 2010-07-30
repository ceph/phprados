PHP_ARG_ENABLE(librados,
    [Enable the librados extension],
    [  --enable-librados      Enable "librados" extension support])

if test $PHP_LIBRADOS != "no"; then
    PHP_REQUIRE_CXX()
    PHP_SUBST(LIBRADOS_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, LIBRADOS_SHARED_LIBADD)
    PHP_NEW_EXTENSION(librados, librados.cc, $ext_shared)
fi
