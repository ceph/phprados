/*
 * Object Oriented interface part of the phprados PHP5 extension using librados
 *
 * Copyright (C) 2013 Jeroen Moors <jeroen.moors@fluoline.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

<?php

class RadosTest extends PHPUnit_Framework_TestCase {
	
	/**
      * Test if it's possible to create a Rados object
     */    
	public function testRadosCreateObject() {
		$object = new Rados();
		$this->assertNotNull($object);
        
        return $object;
	}
	
	    
	/**
      * Try to set and get config options
      *
      * @depends testRadosCreateObject
     */
	public function testRadosSetOption($object) {
		$option    = 'mon_host';
		$setValue  = getenv($option);
		
		$object->setOption($option, $setValue);
		$getValue = $object->getOption($option);
        
        $this->assertEquals($setValue, $getValue);
    }

	
	/**
      * This test connects to the environment variable 'mon_host' with they key 'key'
      *
      * @depends testRadosCreateObject
     */	
	public function testRadosConnect($object) {
		
		$object->setOption('mon_host', getenv('mon_host'));
		$object->setOption('key', getenv('key'));
		$conn = $object->connect();

        $this->assertTrue($conn);

        return $object;
    }

	/**
      * This test connects to the environment variable 'mon_host' with they key 'key'
      * using the short hand method of connect. Also selects pool $pool as it's pool.
      *      
      */	
	public function testRadosConnectShortHand() {
		$host = getenv('mon_host');
		$key  = getenv('key');
		
        $object = new Rados();
		$conn = $object->connect($host, $key);

        $this->assertTrue($conn);
    }
    
    /**
      * This test creates a pool
      *
      * @depends testRadosConnect
     */	
	public function testRadosCreatePool($object) {
		
		$result = $object->createPool('oounittest');
		
        $this->assertTrue($result);

        return $object;
    }


    /**
      * This test selects a pool
      *
      * @depends testRadosCreatePool
     */	
	public function testRadosSelectPool($object) {
		
		$result = $object->selectPool('oounittest');
		
        $this->assertTrue($result);

        return $object;
    }

    /**
      * This test writes data to the pool oounittest
      *
      * @depends testRadosSelectPool
     */	
	public function testRadosWriteData($object) {
		
		$result = $object->write('unique-id','test_datum');
		
        $this->assertTrue($result);

        return $object;
    }
    
    /**
      * This test reads data from the pool oounittest
      *
      * @depends testRadosWriteData
     */	
	public function testRadosReadData($object) {
		
		$data = $object->read("unique-id");
		
        $this->assertEquals("test_datum", $data);
        
        return $object;
    }

    /**
      * This test removes a pool
      *
      * @depends testRadosReadData
     */	
	public function testRadosDestroyPool($object) {
		
		$result = $object->destroyPool('oounittest');
		
        $this->assertTrue($result);

        return $object;
    }

}

?>
