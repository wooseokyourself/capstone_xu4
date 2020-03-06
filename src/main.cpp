#include <string>

#include "web.hpp"
#include "ml.hpp"
#include "socket.hpp"


int main (void) {
    while (true) {
        struct protocol* dataPtr = RecvBuffer ();
		MachineLearning (dataPtr);
    }
    return 0;
}
