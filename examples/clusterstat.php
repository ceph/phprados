<?php

$rados = rados_create();

rados_conf_read_file($rados, "/etc/ceph/ceph.conf");
rados_connect($rados);

var_dump(rados_cluster_stat($rados));

rados_shutdown($rados);

?>
