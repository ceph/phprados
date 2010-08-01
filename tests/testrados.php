<?php

$rados = new Rados();
$rados->initialize();

$poolname = "gfx";
echo "Test #1: Open pool ".$poolname." and close it again.\n";
$pool = $rados->open_pool($poolname);
echo "\tOpen: ".trim(var_dump($pool))."\n";

echo "\tClose: ".trim(var_dump($rados->close_pool($pool)))."\n";

echo "Test #2: Lookup pool ".$poolname."\n";
echo "\t".trim(var_dump($rados->lookup_pool($poolname)))."\n";

$newpoolname = "phprados";
$snapname = "snap1";
echo "Test #3: Create pool ".$newpoolname." (snapshot it) and remove it again\n";
echo "Create: ".trim(var_dump($rados->create_pool("phprados")))."\n";
$newpool = $rados->open_pool($newpoolname);
echo "\tCreate (snap): ".trim(var_dump($rados->snap_create("snap1", $newpool)))."\n";
echo "\tRemove (snap): ".trim(var_dump($rados->snap_remove("snap1", $newpool)))."\n";
echo "Open: ".trim(var_dump($newpool))."\n";
echo "Remove: ".trim(var_dump($rados->delete_pool($newpool)))."\n";

echo "Test #4: List all pools\n";
var_dump($rados->list_pools());

