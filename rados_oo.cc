#include "rados/librados.h"
#include <stdio.h>
#include <stdbool.h>
#ifdef ZTS
#include "TSRM.h"
#endif
#include "rados_oo.h"

Rados::Rados() {
    //fprintf(stderr, "Constructed CC Class!\n");
    int err;
    //this->hasConfig = 0;
    //this->isConnected = 0;
    err = rados_create(&this->cluster, NULL);
}

Rados::~Rados() {
    //fprintf(stderr, "Destructed CC Class!\n");
    rados_ioctx_destroy(this->io);
    rados_shutdown(this->cluster);
}

int Rados::readConfig(char *filename) {
    //fprintf(stderr, "readConfig called with: %s\n", filename);
    int err;
    err = rados_conf_read_file(this->cluster, filename);
    if (err < 0) {
        //fprintf(stderr, "%s: cannot read config file: %s\n", filename, strerror(-err));
        return 0;
    }
    //this->hasConfig = 1;
    return 1;
}

int Rados::connect(char *poolname) {
    //fprintf(stderr, "Connect called!\n");
    int err;
    err = rados_connect(this->cluster);
    if (err < 0) {
        //fprintf(stderr, "Cannot connect to cluster: %s\n", strerror(-err));
        return 0;
    }
    //this->isConnected = 1;

    // TODO: Verify if the pool we try to connect to exists!
    err = rados_ioctx_create(this->cluster, poolname, &this->io);
    if (err < 0) {
        //fprintf(stderr, "Cannot open rados pool %s: %s\n", poolname, strerror(-err));
        rados_shutdown(cluster);
        return -1;
    }
    return 1;
}

int Rados::write(char *key, char *value,  size_t value_len) {
    int err;
    //fprintf(stderr, "Write Key: %s // Value: %s (%i)\n", key, value, value_len);
    err = rados_write_full(io, key, value, value_len);
    if (err < 0) {
            //fprintf(stderr, "Cannot write: %s\n", strerror(-err));
            this->~Rados();
            return -1;
    }
}

size_t Rados::getSize(char *key) {
    int err;
    uint64_t size;

    err = rados_stat(this->io, key, &size, NULL);
    if (err < 0) {
            //fprintf(stderr, "Cannot stat %s:\n", key, strerror(-err));
            this->~Rados();
    }
    //fprintf(stderr, "Stat Key: %s => Lenght: %d\n", key, size);

    return size;
}

void Rados::read(char *key, char *buffer, uint64_t size) {
    int err;
    uint64_t offset = 0;

    err = rados_read(this->io, key, buffer, size, offset);

    //fprintf(stderr, "Read Key: %s // Value: %s\n", key, buffer);
}
