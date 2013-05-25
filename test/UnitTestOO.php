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
      * This test connect to the environment variable 'mon_host' with they key 'key'
      *
      * @depends testRadosCreateObject
     */	
	public function testRadosConnect($object) {
		
		$object->setOption("mon_host", getenv('mon_host'));
		$object->setOption("key", getenv('key'));
		$conn = $object->connect();

        $this->assertTrue($conn);

        return $object;
    }
    
    /**
      * This test creates a pool
      *
      * @depends testRadosCreateObject
     */	
	public function testRadosCreatePool($object) {
		
		$object->setOption("mon_host", getenv('mon_host'));
		$object->setOption("key", getenv('key'));
		$conn = $object->connect();
		$result = $object->createPool("oounittest");
		
        $this->assertTrue($result);

        return $object;
    }

    /**
      * This test removes a pool
      *
      * @depends testRadosCreatePool
     */	
	public function testRadosDestroyPool($object) {
		
		$object->setOption("mon_host", getenv('mon_host'));
		$object->setOption("key", getenv('key'));
		$conn = $object->connect();
		$result = $object->destroyPool("oounittest");
		
        $this->assertTrue($result);

        return $object;
    }
    
    
}

?>
