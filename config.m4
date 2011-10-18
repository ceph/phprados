PHP_ARG_ENABLE(rados, [Enable the rados extension],
    [  --enable-rados[=DIR]      Enable "RADOS" extension support], no)

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
        AC_MSG_ERROR([Please reinstall the RADOS library from http://ceph.newdream.net])
    else
        PHP_ADD_LIBRARY_WITH_PATH(rados, $RADOS_DIR/$PHP_LIBDIR, RADOS_SHARED_LIBADD)
        PHP_ADD_INCLUDE($RADOS_DIR/include)
    fi

    AC_MSG_CHECKING([for libatomic-ops files (atomic_ops.h)])
    for i in $PHP_RADOS /usr/local /usr; do
        if test -r $i/include/atomic_ops.h; then
            ATOMIC_DIR=$i
            AC_MSG_RESULT(found in $i)
            break
        fi
    done

    if test -z "$ATOMIC_DIR"; then
        AC_MSG_RESULT([not found])
        AC_MSG_ERROR([Please reinstall the libatomic-ops library!])
    else
        PHP_ADD_INCLUDE($ATOMIC_DIR/include)
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

    PHP_SUBST(RADOS_SHARED_LIBADD)
    PHP_NEW_EXTENSION(rados, [rados.c], $ext_shared)
fi