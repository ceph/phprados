#include "rados/librados.h"
#include <stdio.h>
#include <stdbool.h>
#ifdef ZTS
#include "TSRM.h"
#endif
#include "rados_oo.h"

/**
 * Constructor of the Rados class
 *
 */
Rados::Rados() {
	int err;
	this->io = NULL;
	this->hasKey = false;
	this->hasMonitor = false;
	this->isConnected = false;
	this->isReadyForIO = false;
	this->isReadyForConnect = false;
	this->lastErrorMessage = "";
	this->lastErrorNumber = 0;
	err = rados_create(&this->cluster, NULL);
}

/**
 * Destructor of the Rados class
 *
 */
Rados::~Rados() {
	//fprintf(stderr, "Destructing CC Class!\n");
	if (this->isReadyForIO == true) {
		rados_ioctx_destroy(this->io);
	}
	rados_shutdown(this->cluster);
}

/**
 * Sets a rados configuration option
 *
 * @param	option 	Configuration option: mon_host, key, ...
 * @param	value	Set this value for option
 * @return  true if the value was successful set
 */
int Rados::setOption(char *option, char *value) {
	int err;
	
	// Can't set options after connection
    if (this->isConnected) {
		this->setError(0, "Can't set options after a connection has been established.\n");
		return false;
	}

	// Set option, if it fails return false
	err = rados_conf_set(this->cluster, option, value);
    if (err < 0) {
		this->setError(err, "Failed to set requested option.\n");
		return false;
    }
	
	// Monitor host set successfull and did we really set a value?
	if ((strcmp(option, "mon_host") == 0) && strlen(value)) {
		this->hasMonitor = true;
	}
	
	// Key option set successfull?
	if ((strcmp(option, "key") == 0) && strlen(value)) {
		this->hasKey = true;
	}
	
	// All options required to connect set?
	if ((this->hasMonitor) && (this->hasKey)) {
		this->isReadyForConnect = true;
	}
		
	return true;
}

/**
 * Gets a rados configuration option
 *
 * @param	option 	Configuration option: mon_host, key, ...
 * @param	value	Buffer were the result will be stored
 * @return  true if the option was successful retrieved
 */
int Rados::getOption(char *option, char *value) {
	int err; 

	err = rados_conf_get(this->cluster, option, value, 256);
    if (err < 0) {
		return false;
    }
    
	return true;
}

/**
 * Reads a Ceph configuration file
 *
 * @param	filename 	Configuration file to be read
 * @return  true if the configuration was successful read
 */
int Rados::readConfig(char *filename) {
	int err;
	err = rados_conf_read_file(this->cluster, filename);

	if (err < 0) {
        char error[1024];
        snprintf ( error, 1024, "Invalid configuration file %s", filename);
        this->setError(err, error);
        return false;
	}
	
    this->hasKey = true;
    this->hasMonitor = true;
	this->isReadyForConnect = true;
	
	return true;
}

/**
 * Connect to a Ceph cluster
 *
 * @return  true if the connection was successful established
 */
int Rados::connect() {
	int err;
	
	if (this->isReadyForConnect != true) {
		this->setError(0, "Not ready for connection. Not all required options are provided with setOption or readConfig.\n");
		return false;
	}
	
	if (this->isConnected != true) {
		err = rados_connect(this->cluster);
		if (err < 0) {
            if (err == -1) {
                this->setError(err, "Authentication failure, verify authentication key.\n");
			} else if (err == -2) {
                this->setError(err, "Can't correctly read configuration or keyring file.\n");
			} else if (err == -22) {
                this->setError(err, "Invalid connection parameters provided.\n");
            } else {
                //fprintf(stderr, "Unknown error: %d (%s)\n", err, strerror(-err));
                this->setError(err, "Unknown error occurred.\n");            
            }
            return false;
		}
		this->isConnected = true;
	}
	
	return true;
}

/**
 * Select a Ceph pool to work with
 *
 * @return  true if the pool was selected successful
 */
int Rados::selectPool(char *poolname) {
	int err;
    
    if (!this->isConnected) {
        this->setError(0, "Not connected to any cluster.\n");
        return false;
    }
	
    err = rados_ioctx_create(this->cluster, poolname, &this->io);
	if (err < 0) {
        char error[1024];
        snprintf (error, 1024, "Failed to select pool '%s'. %s.\n", poolname, strerror(-err));
        this->setError(err, error);
		return false;
	} else {
		this->isReadyForIO = true;
		return true;
	}
}

/**
 * Write data to the current cluster and pool
 *
 * @param	key			Key used as an identifier for the data provided in value
 * @param	value		Value to be written to the cluster
 * @param	value_len	Length of the value that will be written to the cluster
 * @return  true if the connection was successful established
 */
int Rados::write(char *key, char *value,  size_t value_len) {
	int err;
	if (this->isReadyForIO != true) {
		this->setError(0, "Can't write before a pool is selected!\n"); 
		return false;
	}
	
	err = rados_write_full(io, key, value, value_len);
	if (err < 0) {
        char error[1024];
        snprintf (error, 1024, "Can't write key %s: %s\n", key, strerror(-err));
        this->setError(err, error);
		return false;
	}
    
    return true;
}

/**
 * Read data from the cluster
 *
 * @param	key		Identifier for the data to be retrieved
 * @param	buffer	Buffer used to store the retrieved data
 * @param	size	Size of the retrieved data
 * @return  true if the data was successful retrieved
 */
int Rados::read(char *key, char *buffer, uint64_t size) {
	int err;
	uint64_t offset = 0;
	
	if (size < 1) {
		fprintf(stderr, "Can't read size < 1!\n");
		return false;
	}
	
	if (this->isReadyForIO != true) {
		fprintf(stderr, "Can't read until a pool is selected!\n");
		return false;
	}

	err = rados_read(this->io, key, buffer, size, offset);
	return true;
	//fprintf(stderr, "Read Key: %s // Value: %s\n", key, buffer);
}

/**
 * Get the size of an item
 *
 * @param	key			Key used as identifier for the item
 * @return  The size of the requested object, returns false if the item can't be found
 */
size_t Rados::getSize(char *key) {
	int err;
	uint64_t size;
	
	if (this->isReadyForIO != true) {
		fprintf(stderr, "Can't get size until a pool is selected!\n");
		return false;
	}
	
	err = rados_stat(this->io, key, &size, NULL);
	if (err < 0) {
		fprintf(stderr, "Cannot stat %s:\n", key, strerror(-err));
		return false;
	}	

	return size;	
}

/**
 * Create a new pool
 *
 * @param	pool_nam	Name of the pool that will be created
 * @return  true if the pool was successfully created
 */
int Rados::createPool(char *pool_name) {
	int err;
	if (!this->isConnected) {
		this->setError(0, "Can't create pool when not connected to cluster.\n");
		return false;
	}
	err = rados_pool_create(cluster, pool_name);
	if (err < 0) {
		this->setError(err, strerror(-err));
		return false;
	} else {
		return true;
	}
}

/**
 * Destroys an existing pool
 *
 * @param	pool_nam	Name of the pool that will be destroyed
 * @return  true if the pool was successfully destroyed
 */
int Rados::destroyPool(char *pool_name) {
	int err;
	err = rados_pool_delete(cluster, pool_name);
	if (err < 0) {
		return false;
	} else {
		return true;
	}	
}

/**
 * Set an error number and message
 *
 * @param	number	Numerical identifier for the error to set
 * @param	error	Textual error message 
 */
void Rados::setError(int number, char *error) {
	this->lastErrorNumber = number;
	//fprintf(stderr, "\n!! Error: %s !!\n\n", error);
	this->lastErrorMessage = error;
}
