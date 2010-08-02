--TEST--
Rados::initialize rados test
--SKIPIF--
<?php !extension_loaded('rados'); ?>
--FILE--
<?php
$rados = new Rados;
var_dump($rados);
?>
--EXPECTF--
object(Rados)#%d (0) {
}
