<?php

$r = new Rados();
$r->initialize();

foreach ($r->list_pools() as $pool) {
    echo $pool."\n";
}

?>