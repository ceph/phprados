<?php

$poolname = "phprados";

$r = new Rados();
$r->initialize();

$r->create_pool($poolname);

$pool = $r->open_pool($poolname);

for($i = 0; $i < 100; $i++) {
    $objname = md5(rand(0,1000));
    $r->create($pool, $objname);
    $r->write_full($pool, $objname, md5(rand(0,100000)));
    echo "Created object ".$objname."\n";
}

/* Native RADOS calls */
$ctx = $r->list_objects_open($pool);
var_dump(($r->list_objects_more($ctx, 1024)));
$r->list_objects_close($ctx);

/* Or the simple (limited to 1024 objects) PHP way */
var_dump($r->list_objects($pool));

$r->close_pool($pool);

?>