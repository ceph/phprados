<?php

$rados = rados_create();

rados_conf_read_file($rados, "/etc/ceph/ceph.conf");
rados_connect($rados);

var_dump(rados_pool_lookup($rados, "rbd"));
var_dump(rados_pool_reverse_lookup($rados, 36));

rados_shutdown($rados);

?>
