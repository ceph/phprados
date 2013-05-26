<?php
	print __LINE__."\tCreate a new object\n";
	$rados = new Rados();
	
	print __LINE__."\tRead the ceph configuration file\n";
	$rados->readConfig("/etc/ceph/ceph.conf");
	
	print __LINE__."\tConnect to the cluster\n";
	$rados->connect();
		
	print __LINE__."\tSelect pool 'example'\n";        
    $rados->selectPool('example');
    
	print __LINE__."\tWrite some data...'\n";
	$rados->write("unique-id","Some random data\n");
	
	print __LINE__."\tRead the data...\n";
	print $rados->read("unique-id");
