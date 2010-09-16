BEGIN_EXTERN_C()
static php_stream* rados_wrapper_open_url(php_stream_wrapper *wrapper, char *path, char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
static int rados_wrapper_rename(php_stream_wrapper *wrapper, char *url_from, char *url_to, int options, php_stream_context *context TSRMLS_DC);
static int rados_wrapper_unlink(php_stream_wrapper *wrapper, char *url, int options, php_stream_context *context TSRMLS_DC);
static int rados_wrapper_stat(php_stream_wrapper *wrapper, char *url, int flags, php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC);

static size_t rados_stream_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC);
static size_t rados_stream_read( php_stream *stream, char *buf, size_t count TSRMLS_DC);
static int    rados_stream_close(php_stream *stream, int close_handle TSRMLS_DC);
static int    rados_stream_flush(php_stream *stream TSRMLS_DC);
static int    rados_stream_seek( php_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC);
static int    rados_stream_stat( php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC);
END_EXTERN_C()