#include "GNGServer.h"



GNGConfiguration GNGServer::current_configuration = GNGConfiguration::getDefaultConfiguration();
boost::mutex GNGServer::static_lock;

