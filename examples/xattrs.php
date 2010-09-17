<?php

$r = new Rados();
$r->initialize();

$pool = $r->open_pool("phprados");

$objname = "testobject";

$r->create($pool, $objname);

$data = file_get_contents("/etc/passwd");
$r->write_full($pool, $objname, $data);

$r->setxattr($pool, $objname, "content_type", "text/plain");
$r->setxattr($pool, $objname, "md5sum", md5($data));

echo "Dumping the \"content_type\" attribute: ".$r->getxattr($pool, $objname, "content_type")."\n";

var_dump($r->getxattrs($pool, $objname));

?>