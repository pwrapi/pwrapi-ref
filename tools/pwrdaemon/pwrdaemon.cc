
#include <pthread.h>
#include <string.h>
#include "router.h"
#include "server.h"


void* startRtrThread( void *);
void* startSrvrThread( void *);

struct Args {
	int argc;
	std::vector<char*> argv;
};

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void findArgs( std::string prefix, int argc, char** argv, Args& args );

	struct Args srvrArgs;
int main( int argc, char* argv[] )
{
	int rc;
	struct Args rtrArgs;
	pthread_t rtrThread = 0;
	pthread_t srvrThread = 0;
	
	rtrArgs.argv.push_back( argv[0] );
	findArgs( "rtr", argc, argv, rtrArgs );
	pthread_mutex_init(&mutex,NULL);

	if ( rtrArgs.argv.size() > 2 ) {
		rc = pthread_mutex_lock(&mutex);	
		assert(0==rc);
		rc = pthread_create( &rtrThread, NULL, startRtrThread, &rtrArgs );  
		assert(0==rc);
	}

	srvrArgs.argv.push_back( argv[0] );
	findArgs( "srvr", argc, argv, srvrArgs );

	if ( srvrArgs.argv.size() > 2 ) {
		rc = pthread_mutex_lock(&mutex);	
		assert(0==rc);
		rc = pthread_create( &srvrThread, NULL, startSrvrThread, &srvrArgs );  
		assert(0==rc);
	}

	if ( rtrThread ) {
		printf("wait for router thread to exit\n");
		rc = pthread_join( rtrThread, NULL );
		assert(0==rc);
	}

	if ( srvrThread ) {
		printf("wait for server thread to exit\n");
		rc = pthread_join( srvrThread, NULL );
		assert(0==rc);
	}

	return 0;
}

void* startRtrThread( void * _args)
{
	Args& args = *(Args*)_args;
	printf("start router\n");

	PWR_Router::Router rtr(args.argc, &args.argv[0] );

	int rc = pthread_mutex_unlock(&mutex);	
	assert(0==rc);

	return (void*) (unsigned long)rtr.work();
}

void* startSrvrThread( void * _args)
{
	Args& args = *(Args*)_args;

	printf("start server\n");

	PWR_Server::Server srvr(args.argc, &args.argv[0] );

	return (void*) (unsigned long)srvr.work();
}

void findArgs( std::string prefix, int argc, char* argv[], Args& args )
{
	prefix = "--" + prefix + ".";
	int len = prefix.size();

	//printf("prefix=%s\n",prefix.c_str());
	for ( int i = 0; i < argc; i++ ) {
		if ( 0 == strncmp(argv[i],prefix.c_str(),len) ) {

			//printf("'%s' -> ",argv[i]);
			memset( argv[i], '-', len );
			
			char *p = argv[i] + ( len - 2 );
			//printf("'%s'\n",p);
			args.argv.push_back( p );
		}
	}
	args.argc = args.argv.size();
	args.argv.push_back(NULL);
}
