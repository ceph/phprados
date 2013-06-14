PHP_ARG_ENABLE(rados, [Enable the rados extension],
    [  --enable-rados[=DIR]      Enable "RADOS" extension support], no)

PHP_ARG_ENABLE(procedural, [Enable the rados procedural interface],
    [  --enable-procedural     Enable "RADOS" procedural interface], yes, no)

PHP_ARG_ENABLE(oo, [Enable the rados OO interface],
    [  --enable-oo             Enable "RADOS" OO interface], no, no)

if test $PHP_RADOS != "no"; then

    AC_MSG_CHECKING([for RADOS files (librados.h)])
    for i in $PHP_RADOS /usr/local /usr; do
        if test -r $i/include/rados/librados.h; then
            RADOS_DIR=$i
            AC_MSG_RESULT(found in $i)
            break
        fi
    done

    if test -z "$RADOS_DIR"; then
        AC_MSG_RESULT([not found])
        AC_MSG_ERROR([Please reinstall the RADOS library from http://www.ceph.com])
    else
        PHP_ADD_LIBRARY_WITH_PATH(rados, $RADOS_DIR/$PHP_LIBDIR, RADOS_SHARED_LIBADD)
        PHP_ADD_INCLUDE($RADOS_DIR/include)
    fi

    AC_MSG_CHECKING([for libSSL files (crypto.h)])
    for i in $PHP_RADOS /usr/local /usr; do
        if test -r $i/include/openssl/crypto.h; then
            CRYPTO_DIR=$i
            AC_MSG_RESULT(found in $i)
            break
         fi
    done

    if test -z "$CRYPTO_DIR"; then
        AC_MSG_RESULT([not found])
        AC_MSG_ERROR([Please reinstall the libssl library from http://www.openssl.org/])
    else
        PHP_ADD_LIBRARY_WITH_PATH(crypto, $CRYPTO_DIR/$PHP_LIBDIR, RADOS_SHARED_LIBADD)
    fi
	
	if test $PHP_PROCEDURAL == "yes"; then
		AC_DEFINE(BUILD_PROCEDURAL, 1, [ Whether to build the procedural api])
	fi

    if test $PHP_OO == "yes"; then
        AC_DEFINE(BUILD_OO, 1, [Whether to build the OO api])
    fi

	if test $PHP_OO != "no"; then
		PHP_SUBST(RADOS_SHARED_LIBADD)
		PHP_REQUIRE_CXX()
		PHP_SUBST(VEHICLES_SHARED_LIBADD)
		PHP_ADD_LIBRARY(stdc++, 1, RADOS_SHARED_LIBADD)
		PHP_NEW_EXTENSION(rados, [php_rados.cc rados_oo.cc], $ext_shared)
	else
		PHP_SUBST(RADOS_SHARED_LIBADD)
		PHP_NEW_EXTENSION(rados, [php_rados.cc], $ext_shared)
	fi
fi