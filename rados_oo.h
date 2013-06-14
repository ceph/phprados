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
		int setOption(char *option, char *value);
		int getOption(char *option, char *value);
		int connect();
		int readConfig(char *filename = NULL);
		int selectPool(char *poolname);
		int createPool(char *poolname);
		int destroyPool(char *poolname);
		int write(char *key, char *value, size_t value_len);
		int read(char *key, char *bufffer, uint64_t size);
		size_t getSize(char *key);
		char *lastErrorMessage;
		int lastErrorNumber;
	private:
		rados_t cluster;
		rados_ioctx_t io;
		int hasKey;
		int hasMonitor;
		int isReadyForConnect;
		int isConnected;
		int isReadyForIO;
		void setError(int number, char *error);
};

#endif
