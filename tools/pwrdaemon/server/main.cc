#include "server.h"

int main( int argc, char* argv[] ) 
{
	PWR_Server::Server server( argc, argv );

	return server.work();
}
