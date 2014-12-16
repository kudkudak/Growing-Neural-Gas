#include "threading.h"

namespace gmum {

void sleep(int ms) {
	tthread::this_thread::sleep_for(tthread::chrono::milliseconds(ms));
}

}
