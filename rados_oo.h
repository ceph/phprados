#ifndef RADOS_OO_H
#define RADOS_OO_H

#include "rados/librados.h"
#include <stdbool.h>
#ifdef ZTS
	#include "TSRM.h"
#endif

class Rados {
	public:
		Rados();
		~Rados();
		int connect(char *poolname = NULL);
		int readConfig(char *filename = NULL);
		int write(char *key, char *value, size_t value_len);
		void read(char *key, char *bufffer, uint64_t size);
		size_t getSize(char *key);
	private:
		rados_t cluster;
		rados_ioctx_t io;
		//int isConnected;
		//int hasConfig;
};

#endif
