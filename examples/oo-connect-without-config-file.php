<?php
    /*
     * CONNECT TO CEPH USING WITHOUT AN EXTERNAL CONFIG FILE
     * 
     * This example connects to a ceph cluster using functions 
     * to configure the connection parameter.
     *  
     * Make sure to update the $mon_host, $key and $pool to 
     * match your Ceph setup.
     * 
     * At the bottom of this example there's a short hand  
     * connect method as well.
     * 
     */
    
    $mon_host = "127.0.0.1";
    $key      = "__YOUR_KEY_GOES_HERE__";
    $pool     = "phprados";
    
    print __LINE__."\tCreate a new object\n";
	$rados = new Rados();

    print __LINE__."\tSet monitor host parameter\n";
	$rados->setOption("mon_host", $mon_host);

    print __LINE__."\tSet authentication key parameter\n";
	$rados->setOption("key", $key);

	print __LINE__."\tConnect to the cluster\n";
	$rados->connect();
	
	print __LINE__."\tSelect a pool 'example'\n";
	$rados->selectPool($pool);

    // The next line is a short cut to the above
    // $rados->connect($mon_host, $key, $pool);
    
