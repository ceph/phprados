/* g++ -lrados -lcrypto -o testrados testrados.cc */
#include <iostream>

using namespace std;

#include "rados/librados.hpp"

using namespace librados;

#define MSG_ERR cerr << "Error: "
#define MSG_INF cout << "Info: "

int main(int argc, const char **argv)
{

    const char spool[5] = "data";
    Rados rados;
    
    if (rados.initialize(argc, argv) < 0) {
        MSG_ERR << "Couldn't initialize RADOS!" << std::endl;
        exit(1);
    }

    pool_t pool;

    int r = rados.open_pool(spool, &pool);
    if (r < 0) {
        MSG_ERR << "Could not open pool: " << spool << std::endl;
        exit(2);
    }

    rados.shutdown();

    MSG_INF << "RADOS succesfully tested!" << std::endl;
    
    return 0;

}