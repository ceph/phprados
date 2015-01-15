<?php

$rados = rados_create();

rados_conf_read_file($rados, "/etc/ceph/ceph.conf");
rados_connect($rados);

$io = rados_ioctx_create($rados, "rbd");

var_dump(rados_ioctx_pool_stat($io));

rados_ioctx_destroy($io);

rados_shutdown($rados);

?>
