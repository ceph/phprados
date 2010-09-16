#define RADOS_STREAM 1
#include "stream.h"

php_stream_ops rados_ops = {
    rados_stream_write, /* write */
    rados_stream_read,  /* read */
    rados_stream_close, /* close */
    rados_stream_flush, /* flush */
    "RADOS stream",
    rados_stream_seek,  /* seek */
    NULL,              /* cast */
    rados_stream_stat,  /* stat */
    NULL, /* set option */
};

php_stream_wrapper_ops rados_stream_wops = {
    rados_wrapper_open_url,
    NULL,                  /* phar_wrapper_close */
    NULL,                  /* phar_wrapper_stat, */
    rados_wrapper_stat,     /* stat_url */
    rados_wrapper_open_dir, /* opendir */
    "RADOS",
    rados_wrapper_unlink,   /* unlink */
    rados_wrapper_rename,   /* rename */
    rados_wrapper_mkdir,    /* create directory */
    rados_wrapper_rmdir,    /* remove directory */
};

php_stream_wrapper php_stream_rados_wrapper = {
    &rados_stream_wops,
    NULL,
    0 /* is_url */
};

static php_stream * rados_wrapper_open_url(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{

}

static int rados_stream_close(php_stream *stream, int close_handle TSRMLS_DC)
{
    return 0;
}

static size_t rados_stream_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{

}

static int rados_stream_seek(php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC)
{
    return 1;
}

static size_t rados_stream_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{

}

static int rados_stream_flush(php_stream *stream TSRMLS_DC)
{
    return 1;
}

static int rados_stream_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC)
{
    return 1;
}

static int rados_wrapper_stat(php_stream_wrapper *wrapper, char *url, int flags, php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC)
{
    return 1;
}

static int rados_wrapper_unlink(php_stream_wrapper *wrapper, char *url, int options, php_stream_context *context TSRMLS_DC)
{
    return 1;
}

static int rados_wrapper_rename(php_stream_wrapper *wrapper, char *url_from, char *url_to, int options, php_stream_context *context TSRMLS_DC)
{
    return 1;
}
