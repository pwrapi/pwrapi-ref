#include <Python.h>
#include <sys/wait.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/utsname.h>

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <set>

#include <mpi.h>

static int _debug = 0;

struct Data 
{
	Data() : client(-1), daemon(-1) {} 
	pid_t client;	
	pid_t daemon;	
	std::string routeFile;
};

Data* __data = NULL;

static void sighandler( int sig )
{
	if ( _debug ) {
		printf("PWRRT: got signale=%d\n",sig);
	}
}

static std::string createNidList( int& numNodes, int& myNid ); 
static Data* runtimeInit( int *argc, char ***argv, 
				const std::string& nidList, int numNodes, int myNid );

int MPI_Init( int *argc, char ***argv )
{
	int numNodes; 
	int myNid;

	if ( getenv( "POWERRT_DEBUG" ) ) {
		_debug = atoi(getenv( "POWERRT_DEBUG" ) );
	}
	int mpi_retval = PMPI_Init( argc, argv );
	if ( mpi_retval != MPI_SUCCESS ) {
		return mpi_retval;
	}

	std::string nidList = createNidList( numNodes, myNid );

	if ( _debug ) {
		printf("PWRRT: numNodes=%d myNid=%d nidlist `%s`\n",
						numNodes,myNid,nidList.c_str());
	}

	if ( ! nidList.empty() ) {
		__data = runtimeInit( argc, argv, nidList, numNodes, myNid );
	}

	return mpi_retval;
}

Data* runtimeInit( int *argc, char ***argv, 
			const std::string& nidList, int numNodes, int myNid )
{
    int my_rank;
	int numRanks;
	int rc;

	struct sigaction act;
	act.sa_handler = sighandler;
	rc = sigaction( SIGUSR1, &act, NULL );
	assert( 0 == rc );
	rc = sigaction( SIGCHLD, &act, NULL );
	assert( 0 == rc );

    rc =  MPI_Comm_rank( MPI_COMM_WORLD,&my_rank);
	assert( MPI_SUCCESS == rc );

    rc =  MPI_Comm_size( MPI_COMM_WORLD,&numRanks);
	assert( MPI_SUCCESS == rc );

	std::stringstream ret;
	ret << numNodes;
	setenv( "POWERRT_NUMNODES", &ret.str().c_str()[0], 1 );

 	assert( ! __data );
	Data* data = new Data;

	char* tmp = getenv("POWERRT_SCRIPT");
	if ( ! tmp ) {
		printf("ERROR: POWER_SCRIPT is not set\n");
		exit(-1);
	}
	assert(tmp);
	std::string file(tmp);

    size_t pos = file.find_last_of('/');

    std::string path;
    std::string moduleName = file;
    if ( std::string::npos != pos ) {
        path = file.substr( 0, pos );
        moduleName = file.substr( pos + 1 );

        setenv( "PYTHONPATH", path.c_str(), 1 );
    }

    moduleName = moduleName.substr(0, moduleName.find_last_of('.' ) );

	if ( _debug && 0 == my_rank ) {
    	printf( "PWRRT: path=`%s` module=`%s`\n", path.c_str(),
											moduleName.c_str() );
	}

	Py_Initialize();

	PyObject* module = PyImport_ImportModule( moduleName.c_str() ) ;
	if ( ! module ) {
		fprintf(stderr, "ERROR: PyImport_ImportModule( `%s` ) failed\n",
											moduleName.c_str());
		exit(1);
	}

    PyObject* pFunc = PyObject_GetAttrString( module, "GetApps" );
    assert(pFunc);

    PyObject* pArgs = PyTuple_New( 9 );
    assert(pArgs);

	std::stringstream routeFile;
	// create a unique name for the route file
	routeFile << "routeTable." << getpid() << "." << myNid;

	char* config = getenv("POWERAPI_CONFIG"); 
	if ( ! config ) {
		printf("ERROR: POWERAPI_CONFIG is not set\n");
		exit(-1);
	} 
	data->routeFile = routeFile.str();
    PyTuple_SetItem( pArgs, 0, PyInt_FromLong( myNid ) );
    PyTuple_SetItem( pArgs, 1, PyString_FromString( config ) );
    PyTuple_SetItem( pArgs, 2, PyString_FromString( nidList.c_str() ) );
    PyTuple_SetItem( pArgs, 3, PyString_FromString( routeFile.str().c_str() ) );

	std::string object = "plat";
	if ( getenv("POWERRT_OBJECT") ) { 
		object = getenv("POWERRT_OBJECT");
	}

	std::string attr = "";
	if ( getenv("POWERRT_ATTR") ) { 
		attr = getenv("POWERRT_ATTR");
	}

	std::string logFile = "";
	if ( getenv("POWERRT_LOGFILE") ) { 
		logFile = getenv("POWERRT_LOGFILE");
	}

	if ( ! getenv("POWERRT_DAEMON") ) { 
		printf("ERROR: POWERRT_DAEMON is not set\n");
		exit(-1);
	}

	std::string daemon = getenv("POWERRT_DAEMON");

	std::string client = "";
	if ( getenv("POWERRT_CLIENT") ) { 
		client = getenv("POWERRT_CLIENT");
	}

    PyTuple_SetItem( pArgs, 4, PyString_FromString( object.c_str() ) );
    PyTuple_SetItem( pArgs, 5, PyString_FromString( attr.c_str() ) );
    PyTuple_SetItem( pArgs, 6, PyString_FromString( logFile.c_str() ) );
    PyTuple_SetItem( pArgs, 7, PyString_FromString( daemon.c_str() ) );
    PyTuple_SetItem( pArgs, 8, PyString_FromString( client.c_str() ) );

    PyObject* pRetval = PyObject_CallObject( pFunc, pArgs );
    assert(pRetval);

    for ( int i=0; i < PyList_Size( pRetval); i++ ) {

        PyObject* pyTmp = PyList_GetItem(pRetval,i);
        PyObject* pyArgs = PyList_GetItem(pyTmp,0);
        PyObject* pyEnv = PyList_GetItem(pyTmp,1);

		if ( _debug ) {
			printf("PWRRT: %d ",i); 
			PyObject_Print( pyArgs, stdout, Py_PRINT_RAW ); 
			printf("\n");
			if ( pyEnv ) {
				printf("PWRRT: %d ",i); 
				PyObject_Print( pyEnv, stdout, Py_PRINT_RAW );
				printf("\n");
			}
		}

		std::vector<char*> argv;
		std::vector<std::string> args;
		for ( int j=0; j < PyList_Size( pyArgs ); j++ ) {
			std::string str =PyString_AsString( PyList_GetItem( pyArgs, j ) );
        	args.push_back( str );
		}

		for ( unsigned int j=0; j < args.size(); j++ ) {
        	argv.push_back( &args[j][0] );
		}
       	argv.push_back( 0 );

		std::vector<char*> envp;
		std::vector<std::string> envs;

		if ( pyEnv ) {
			for ( int j=0; j < PyList_Size( pyEnv ); j++ ) {
				std::string str =PyString_AsString( PyList_GetItem( pyEnv, j ) );
        		envs.push_back( str );
			}
		}

		for ( unsigned int j=0; j < envs.size(); j++ ) {
        	envp.push_back( &envs[j][0] );
		}
       	envp.push_back( 0 );

		if ( _debug ) {
			printf("PWRRT: rank=%d launch\n",my_rank);
			for ( unsigned j = 0; j < argv.size(); j++ ) {
				printf("PWRRT:    %s\n",argv[j] );
			}
		}

		int child;
		if ( ( child = fork() ) ) {
			if ( i == 0 ) {
				data->daemon = child;
			} else if ( i == 1 ) {
				data->client = child;
			} else {
				assert(0);
			}
		} else if ( 0 == child )  {
			int rc = execve( argv[0], &argv[0], &envp[0] );
			if ( -1 == rc ) {
				fprintf(stderr,"execve '%s' failed %s\n",
									argv[0],strerror(errno));
				exit(-1);
			}
		} else {
			assert(0);
		}
		if (_debug ) printf("PWRRT: wait for child\n");
		pause();
		if (_debug ) printf("PWRRT: child is ready\n");

#if 0
		// this is causing a failure for some reason
		// it's a small memory leak
        Py_DECREF( pyTmp );
        Py_DECREF( pyArgs );
        Py_DECREF( pyEnv );
#endif
    }
    Py_DECREF(pArgs);
    Py_DECREF(pFunc);
    Py_DECREF(pRetval);
	return data; 
}

int MPI_Finalize()
{
	Data* data = __data;
	
	if ( data ) {

		unlink( data->routeFile.c_str() );
	
		if ( data->client > -1 ) {
			if ( _debug ) {
				printf("PWRRT: kill client %d\n", data->client);
			}
			kill( data->client, SIGKILL );
			waitpid( data->client,NULL, 0  );
		}
		MPI_Barrier( MPI_COMM_WORLD );
		if ( data->daemon > -1 ) {
			if ( _debug ) {
				printf("PWRRT: kill daemon %d\n", data->daemon);
			}
			kill( data->daemon, SIGKILL );
			waitpid( data->daemon,NULL, 0  );
		}
		delete data;
	} else {
		MPI_Barrier( MPI_COMM_WORLD );
	}

	return PMPI_Finalize();
}

static std::string createNidList(  int& numNodes, int& myNid )
{
	int rc;
	int my_rank;
	int numRanks;

    rc =  MPI_Comm_rank( MPI_COMM_WORLD,&my_rank);
	assert( MPI_SUCCESS == rc );

    rc =  MPI_Comm_size( MPI_COMM_WORLD,&numRanks);
	assert( MPI_SUCCESS == rc );

	struct utsname buf;
	rc = uname( &buf );
	assert( 0 == rc );
	
	std::string procName;
	procName.resize(MPI_MAX_PROCESSOR_NAME);
	int len;
	MPI_Get_processor_name( &procName[0], &len );
	procName.resize(len);

	int pos = 0;
	while ( ! isdigit( procName[pos] ) ) { ++pos; }

	int nodeid = atoi( &procName[pos] );
	if ( _debug ) {
		printf("PWRRT: my_rank=%d procName=%s nodeid=%d\n",
						my_rank, procName.c_str(), nodeid );
	}
	
	std::vector<int> rbuf( numRanks );

	rc = MPI_Allgather( &nodeid, 1, MPI_INT, &rbuf[0], 1, 
											MPI_INT, MPI_COMM_WORLD );
	assert( MPI_SUCCESS == rc ); 

	std::vector<int> rbuf2( numRanks );
	int nameLen = procName.size();
	rc = MPI_Allgather( &nameLen, 1, MPI_INT, &rbuf2[0], 1, 
											MPI_INT, MPI_COMM_WORLD );
	assert( MPI_SUCCESS == rc ); 

	int launcherRank = -1; 
	for ( int i = 0; i < numRanks; i++ ) {
		if ( rbuf[i] == nodeid && launcherRank == -1 ) {
			launcherRank = i;
			break;
		}
		if ( nameLen == procName.size() && rbuf2[i] != nameLen ) {
			nameLen = rbuf2[i];
		}
	}

	std::stringstream ret;

	if ( my_rank != launcherRank ) {
		return ret.str();
	} 

	if ( _debug ) {
		printf( "Rank %d is a launcher\n", my_rank );
	}

	std::string prefix=procName.substr(0,pos);

	ret << std::setw(1) << prefix;
	ret << std::setw(1) << "[";
	
	std::set< int > nodes;

	numNodes = 0;
	for ( unsigned i = 0; i < rbuf.size(); i++ ) {
		if ( nodes.find( rbuf[i] ) == nodes.end() ) {
			nodes.insert( rbuf[i] );

			if ( rbuf[i] == nodeid ) {
				myNid = numNodes; 
			} 
			++numNodes;
			if ( _debug && 0 == my_rank ) {
				printf("PWRRT: rank %d -> node %d\n",i,rbuf[i]);
			}

			if ( i > 0 ) {
				ret << std::setw(1) << ","; 		
			}

			if ( nameLen == procName.size() ) {
				ret << std::setw( procName.size() - pos) << 
									std::setfill('0') << rbuf[i];
			} else {
				ret << rbuf[i];
			}
		}
	}

	ret << std::setw(1) << "]";

	return ret.str();
}
