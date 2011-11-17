<?php

$rados = rados_create();

rados_conf_read_file($rados, "/etc/ceph/ceph.conf");
rados_connect($rados);

$io = rados_ioctx_create($rados, "phprados");

rados_write_full($io, "passwd", file_get_contents("/etc/passwd"));

rados_setxattr($io, "passwd", "aap", "attr1");
rados_setxattr($io, "passwd", "noot", "attr2");
rados_setxattr($io, "passwd", "mies", "attr3");

var_dump(rados_getxattrs($io, "passwd"));

rados_ioctx_destroy($io);

rados_shutdown($rados);

?>