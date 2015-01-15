/*
 * phprados - A PHP5 extension for using librados
 *
 * Copyright (C) 2013 Wido den Hollander <wido@widodh.nl>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

<?php

class RadosTest extends PHPUnit_Framework_TestCase {

    public function testRadosConfSetGet() {
        $r = rados_create();
        $this->assertNotNull($r);

        $confOption = "mon_host";
        $confValue = "127.0.0.1";

        rados_conf_set($r, $confOption, $confValue);
        $value = rados_conf_get($r, $confOption);
        $this->assertEquals($confValue, $value);
    }

    public function testRadosConnect() {
        $cluster = rados_create(getenv('id'));
        $this->assertNotNull($cluster);

        rados_conf_set($cluster, "mon_host", getenv('mon_host'));
        rados_conf_set($cluster, "key", getenv('key'));

        $this->assertTrue(rados_connect($cluster));

        return $cluster;
    }

    /**
      * This test will create the pool specified in the environment variable 'pool'
      *
      * After a succesfull creation it will remove the pool again
      *
      * @depends testRadosConnect
     */
    public function testRadosCreatePool($cluster) {
        $this->assertTrue(rados_pool_create($cluster, getenv('pool')));
        return $cluster;
    }

    /**
     * @depends testRadosCreatePool
     */
    public function testRadosCreateIoCTX($cluster) {
        $ioctx = rados_ioctx_create($cluster, getenv('pool'));
        $this->assertNotNull($ioctx);
        return $ioctx;
    }

    /**
     * @depends testRadosCreateIoCTX
     */
    public function testRadosWrite($ioctx) {
        $oid = md5(rand(0,9999999999));
        $buf = sha1($oid);
        $r = rados_write($ioctx, $oid, $buf, 0);
        $this->assertTrue($r);
        $info = array();
        $info['oid'] = $oid;
        $info['buf'] = $buf;
        $info['ioctx'] = $ioctx;
        return $info;
    }

    /**
     * @depends testRadosWrite
     */
    public function testRadosRead($info) {
        $buf = rados_read($info['ioctx'], $info['oid'], strlen($info['buf']));
        $this->assertEquals($buf, $info['buf']);
        return $info;
    }

    /**
     * @depends testRadosRead
     */
    public function testRadosRemove($info) {
        $r = rados_remove($info['ioctx'], $info['oid']);
        $this->assertTrue($r);
        return $info;
    }

    /**
     * @depends testRadosRemove
     */
    public function testRadosPoolStat($info) {
        $stats = rados_ioctx_pool_stat($info['ioctx']);
        $this->assertGreaterThanOrEqual(0, $stats['num_bytes']);
        $this->assertGreaterThanOrEqual(0, $stats['num_kb']);
        $this->assertGreaterThanOrEqual(0, $stats['num_objects']);
        $this->assertGreaterThanOrEqual(0, $stats['num_object_clones']);
        $this->assertGreaterThanOrEqual(0, $stats['num_object_copies']);
        $this->assertGreaterThanOrEqual(0, $stats['num_objects_missing_on_primary']);
        $this->assertGreaterThanOrEqual(0, $stats['num_objects_unfound']);
        $this->assertGreaterThanOrEqual(0, $stats['num_objects_degraded']);
        $this->assertGreaterThanOrEqual(0, $stats['num_rd']);
        $this->assertGreaterThanOrEqual(0, $stats['num_rd_kb']);
        $this->assertGreaterThanOrEqual(0, $stats['num_wr']);
        $this->assertGreaterThanOrEqual(0, $stats['num_wr_kb']);
        return $info;
    }

    /**
     * @depends testRadosPoolStat
     */
    public function testRadosDestroyIoCTX($info) {
        $this->assertNull(rados_ioctx_destroy($info['ioctx']));
    }

    /**
     * @depends testRadosCreatePool
     */
    public function testRadosDeletePool($cluster) {
        $this->assertTrue(rados_pool_delete($cluster, getenv('pool')));
    }

    /**
     * @depends testRadosConnect
     */
    public function testRadosPoolList($cluster) {
        $this->assertNotNull(rados_pool_list($cluster));
    }

    /**
     * @depends testRadosConnect
     */
    public function testRadosClusterStat($cluster) {
        $stats = rados_cluster_stat($cluster);
        $this->assertGreaterThan(0, $stats['kb']);
        $this->assertGreaterThanOrEqual(0, $stats['kb_used']);
        $this->assertLessThan($stats['kb'], $stats['kb_avail']);
        $this->assertGreaterThanOrEqual(0, $stats['num_objects']);
    }

    /**
     * @depends testRadosConnect
     */
    public function testRadosClusterFsid($cluster) {
        $fsid = rados_cluster_fsid($cluster);
        $this->assertEquals(36, strlen($fsid));
    }

    /**
     * @depends testRadosConnect
     */
    public function testRadosShutDown($cluster) {
        $this->assertTrue(rados_shutdown($cluster));
    }
}

?>
