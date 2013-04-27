<?php
	print __LINE__."\tCreate a new object\n";
	$rados = new Rados();
	
	print __LINE__."\tRead the ceph configuration file\n";
	$rados->readConfig("/etc/ceph/ceph.conf");
	//$rados->connect("tata");
	
	print __LINE__."\tConnect to the cluster and select pool 'examples'\n";
	$rados->connect('examples');
		
	print __LINE__."\tWrite some data...'\n";
	$rados->write("unique-id","Some random data\n");
	
	print __LINE__."\tRead the data...\n";
	print $rados->read("unique-id");
