<?php
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

class RadosUnitTest extends PHPUnit\Framework\TestCase {

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
        if (getenv('config')) {
            $cluster = rados_create();
            rados_conf_read_file($cluster, getenv('config'));
        } else {
            $cluster = rados_create(getenv('id'));
            rados_conf_set($cluster, "mon_host", getenv('mon_host'));
            rados_conf_set($cluster, "key", getenv('key'));
        }
        $this->assertNotNull($cluster);

        $this->assertTrue(rados_connect($cluster));

        return $cluster;
    }

    /**
     * @depends testRadosConnect
     */
    public function testRadosInstanceId($cluster) {
        $instance_id = rados_get_instance_id($cluster);
        $this->assertGreaterThan(0, $instance_id);
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
    public function testRadosGetIoCTXId($ioctx) {
        $ioctx_id = rados_ioctx_get_id($ioctx);
        $this->assertGreaterThan(0, $ioctx_id);
        return $ioctx;
    }

    /**
     * @depends testRadosGetIoCTXId
     */
    public function testRadosIoCTXPoolName($ioctx) {
        $this->assertEquals(getenv('pool'), rados_ioctx_get_pool_name($ioctx));
        return $ioctx;
    }

    /**
     * @depends testRadosGetIoCTXId
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
    public function testRadosPoolLookup($cluster) {
        $pool = getenv('pool');
        $id = rados_pool_lookup($cluster, $pool);
        $this->assertGreaterThanOrEqual(0, $id);
        $info['cluster'] = $cluster;
        $info['id'] = $id;
        $info['pool'] = getenv('pool');
        return $info;
    }

    /**
     * @depends testRadosPoolLookup
     */
    public function testRadosPoolReverseLookup($info) {
        $pool = rados_pool_reverse_lookup($info['cluster'], $info['id']);
        $this->assertEquals($pool, $info['pool']);
        return $info;
    }

    /**
     * @depends testRadosPoolReverseLookup
     */
    public function testRadosCreate2IoCTX($info) {
        $ioctx = rados_ioctx_create2($info['cluster'], $info['id']);
        $this->assertNotNull($ioctx);
        $this->assertNull(rados_ioctx_destroy($ioctx));
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
        $this->assertEquals(1, preg_match('/^\w{8}-\w{4}-\w{4}-\w{4}-\w{12}$/', $fsid));
    }

    /**
     * @depends testRadosConnect
     */
    public function testRadosWaitForOSDMap($cluster) {
        $this->assertTrue(rados_wait_for_latest_osdmap($cluster));
    }

    /**
     * @depends testRadosConnect
     */
    public function testRadosShutDown($cluster) {
        $this->assertTrue(rados_shutdown($cluster));
    }

    /**
     * @depends testRadosCreateIoCTX
     */
    public function testRadosNamespace($ioctx) {
        $this->markTestSkipped('The rados_ioctx_set_namespace() function causes segmentation faults.');
        $this->assertNull(rados_ioctx_get_namespace($ioctx));
        rados_ioctx_set_namespace($ioctx, "foo");
        $name = rados_ioctx_get_namespace($ioctx);
        $this->assertEquals($name, "foo");
        rados_ioctx_set_namespace($ioctx, NULL);
        $this->assertNull(rados_ioctx_get_namespace($ioctx));
    }

    /**
     * @depends testRadosCreateIoCTX
     */
    public function testRadosPoolRequiredAlignment($ioctx) {
        $this->assertEquals(0, rados_ioctx_pool_required_alignment($ioctx));
    }
}
