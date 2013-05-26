<?php
    $rados = new Rados();
    $x = $rados->connect("127.012","");
    var_dump($x);
    print $rados->getLastError();
