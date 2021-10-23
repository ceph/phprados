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

PHPUnit can be downloaded as a PHAR archive from the PHPUnit website:

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
    <testsuite name="Rados Test Suite">
      <file>test/RadosUnitTest.php</file>
    </testsuite>
  </testsuites>

  <php>
    <env name="config"   value="/etc/ceph/ceph.conf" />

    <env name="mon_host" value="127.0.0.1" />
    <env name="id"       value="admin" />
    <env name="key"      value="CEPHXSECRET" />

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
PHPUnit 8.5.2 by Sebastian Bergmann and contributors.

.....................S.                                           23 / 23 (100%)

Time: 3.08 seconds, Memory: 4.00 MB

OK, but incomplete, skipped, or risky tests!
Tests: 23, Assertions: 55, Skipped: 1.
```
