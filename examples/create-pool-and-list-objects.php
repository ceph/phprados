<?php

$poolname = "phprados";

$rados = rados_create();
rados_conf_read_file($rados, '/etc/ceph/ceph.conf');
rados_connect($rados);

rados_pool_create($rados, $poolname);

$io = rados_ioctx_create($rados, $poolname);

for($i = 0; $i < 100; $i++) {
    $objname = md5(rand(0,1000));
    rados_write_full($io, $objname, md5(rand(0,100000)));
    echo "Created object ".$objname."\n";
}

/* List all objects */
print_r(rados_objects_list($io));

rados_ioctx_destroy($io);
rados_shutdown($rados);

?>