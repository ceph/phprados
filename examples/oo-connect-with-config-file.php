<?php
    /*
     * CONNECT TO CEPH USING A CONFIG FILE
     * 
     * This example connects to a ceph cluster using a configuration 
     * and keyring file to get the connection parameters.
     *  
     */
    
    print __LINE__."\tCreate a new object\n";
	$rados = new Rados();
    
    print __LINE__."\tRead the ceph configuration file\n";
	$rados->readConfig("/etc/ceph/ceph.conf");
	
	print __LINE__."\tConnect to the cluster\n";
	$rados->connect();
	
	print __LINE__."\tSelect a pool 'example'\n";
	$rados->selectPool("phprados");
