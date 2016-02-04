# PHPRADOS
phprados is a PHP extension for [librados](http://www.ceph.com/docs/master/rados/api/librados/).

The extension wraps the C API of librados into a extension, where almost all the native methods are implemented.

# Limitations
The extension should work on both 32 and 64-bit systems, but due to PHP's engine it might now work well with very large objects.

Also, the selfmanaged snapshots are not implemented in the extension since their usage is pretty rare.

# Methods / Functions
For a full overview of the available functions, open a terminal and run:

```bash
$ php --re rados
```

Real code examples can be taken from the Unit tests.

# Sources
Articles used for this extension:
* http://devzone.zend.com/article/1021
* http://devzone.zend.com/article/1022
* http://devzone.zend.com/article/1024-Extension-Writing-Part-III-Resources
* http://devzone.zend.com/article/4486-Wrapping-C-Classes-in-a-PHP-Extension

# Dependencies
Need libraries (Ubuntu / Debian):
* libatomic-ops-dev (Needed for librados)
* librados2(-dev)

# Help
phprados comes as it is, but there could always be bugs or issues.

For questions, please visit Ceph's IRC channel (#ceph on irc.oftc.net) or contact the maintainers directly.

# Examples
There is an examples directory which contains some examples.

You probably want to look at the unit tests though, since they will be well maintained.

# Unit Testing
Unit tests are available in the "test" directory.

These tests are all written for PHPUnit and require PHPUnit to be installed.

PHPUnit can be dowloaded as a PHAR archive from the PHPUnit website:

```bash
$ wget https://phar.phpunit.de/phpunit.phar
$ chmod +x phpunit.phar
```

Before you can run the tests, create a 'phpunit.xml' based on the 'phpunit.xml.dist' file containing
the connection credentials to the Ceph cluster.

For example:

```
<?xml version="1.0" encoding="UTF-8"?>
<phpunit>
  <testsuites>
    <testsuite name="My Test Suite">
      <file>test/UnitTest.php</file>
    </testsuite>
  </testsuites>

  <php>
    <env name="mon_host" value="localhost" />
    <env name="id"       value="admin" />
    <env name="key"      value="AQBYnaZUyOeRCRAAysjZSIsHO6aWKiTjXkVNMQ==" />
    <env name="pool"     value="phprados" />
  </php>
</phpunit>
```

Afterwards, run PHPUnit:

```bash
$ ./phpunit.phar
```

It should show that the tests have run successfully:

```
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

PHPUnit 4.8.7 by Sebastian Bergmann and contributors.

..................

Time: 2.23 seconds, Memory: 13.25Mb

OK (18 tests, 49 assertions)
```
