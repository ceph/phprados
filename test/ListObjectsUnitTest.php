<?php

class ListObjectsUnitTest extends PHPUnit\Framework\TestCase
{
    private $cluster;

    private $ioContext;

    public function setUp(): void
    {
        if (getenv('config')) {
            $this->cluster = rados_create();
            rados_conf_read_file($this->cluster, getenv('config'));
        } else {
            $this->cluster = rados_create(getenv('id'));
            rados_conf_set($this->cluster, "mon_host", getenv('mon_host'));
            rados_conf_set($this->cluster, "key", getenv('key'));
        }
        rados_connect($this->cluster);
        rados_pool_delete($this->cluster, getenv('pool'));
        rados_pool_create($this->cluster, getenv('pool'));
        $this->ioContext = rados_ioctx_create($this->cluster, getenv('pool'));
    }

    public function tearDown(): void
    {
        rados_pool_delete($this->cluster, getenv('pool'));
    }

    public function testListObjects() {
        $this->givenThereAreObjects(50);
        $this->assertEquals(50, count(rados_objects_list($this->ioContext)));
    }

    public function testListObjectsWithLimit() {
        $this->givenThereAreObjects(50);
        // Get the full list
        $fullList = rados_objects_list($this->ioContext);

        // Match up the first 20
        $first20 = rados_objects_list($this->ioContext, 20);

        $expectedFirst20 = array_splice($fullList, 0, 20);

        $this->assertEquals($expectedFirst20, $first20);
    }


    public function testListObjectsWithStartPosition() {
        $this->givenThereAreObjects(50);
        // Get the full list
        $fullList = rados_objects_list($this->ioContext);
        $startPositionIndex = 30;
        $startPositionItem = $fullList[$startPositionIndex];

        // Get the next 10
        $next10 = rados_objects_list($this->ioContext, 10, $startPositionItem);

        $expectedNext10 = array_splice($fullList, $startPositionIndex + 1, 10);
        $this->assertEquals($expectedNext10, $next10);
    }

    private function givenThereAreObjects($count)
    {
        for ($i = 0; $i < $count; $i++) {
            rados_write_full($this->ioContext, 'file'.$i, uniqid());
        }
    }
}