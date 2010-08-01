<?php

$rados = new Rados();
$rados->initialize();
$pool = $rados->open_pool("gfx");
var_dump($pool);
var_dump($rados->close_pool($pool));