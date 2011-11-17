<?php

$rados = rados_create();
rados_conf_set('mon addr', '1.2.3.4');
rados_conf_set('keyring', '/path/to/keyring.bin');
rados_connect($rados);

print_r(rados_pool_list($rados));

rados_shutdown($rados);

?>