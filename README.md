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

This can be done using PEAR:
```bash
$ pear config-set auto_discover 1
$ pear install pear.phpunit.de/PHPUnit
```

Running the tests is simple:
```bash
$ phpunit
```
