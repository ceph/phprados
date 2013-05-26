<?php
	print __LINE__."\tCreate a new object\n";
	$rados = new Rados();
	
	print __LINE__."\tRead the ceph configuration file\n";
	$rados->readConfig("/etc/ceph/ceph.conf");
	
	print __LINE__."\tConnect to the cluster\n";
	$rados->connect();
	
	print __LINE__."\tCreate a pool 'example'\n";
	$rados->createPool("example");
