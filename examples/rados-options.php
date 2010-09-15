<?php

$options = array(
    'config_file' => '/path/to/ceph.conf', // -c
    'monitor_ip' => '1.2.3.4', // -m
    'cephx_name' => 'admin' // -n
    'cephx_keyfile' => '/path/to/keyfile', // -K
    'cephx_keyring' => '/path/to/keyring.bin', // -k

);

$r = new Rados();
$r->initialize();

var_dump($r->list_pools());

?>