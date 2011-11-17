<?php

$rados = rados_create();
rados_conf_read_file($rados, '/etc/ceph/ceph.conf');
rados_connect($rados);

foreach (rados_pool_list($rados) as $pool) {
    echo $pool."\n";
}

rados_destroy($rados);

?>