Examples
=========
The phprados extension comes in two flavors:
- A procedural interface 
- An object oriented interface.

For both interfaces examples can be found in this directory.

Some examples require a pool named 'example'. Create it in advanced or use
one of the create-pool examples to create this pool.

Procedural interface
--------------------
*   (rados-options.php)
    Connect to a Ceph cluster setting the parameters from the code.
    
*   (create-pool-and-list-objects.php)
    Create a new pool, add some objects and list all objects.
    
*   (list-pools.php)
    List all available pools.

*   (xattrs.php)
    Set and read extended attributes.

Object oriented interface
-------------------------
*   (oo-connect-with-config-file.php)
    Connect to a Ceph cluster using a Ceph configuration file.
    
*   oo-connect-without-config-file.php
    Connect to a Ceph cluster providing the required parameters directly 
    from the code. Both the extended and a short hand way are shown.
     
*   oo-create-pool.php
    Create a new pool.
    
*   oo-destroy-pool.php
    Remove an existing pool.

*   oo-write-read-data.php
    Write data to a Ceph pool
