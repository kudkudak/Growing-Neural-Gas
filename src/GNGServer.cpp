#include "GNGServer.h"



GNGConfiguration GNGServer::current_configuration = GNGConfiguration::getDefaultConfiguration();
boost::mutex GNGServer::static_lock;
int GNGServer::START_NODES = 20; //TODO: change to 1000
