#include <Python.h>
#include <sys/wait.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/utsname.h>

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <set>

#include <mpi.h>

struct Data {
	std::vector<int> child;
};

Data* __data = NULL;

static std::string createNidList( int rank, int& numNodes, int& myNid ); 

static int _debug = 0;
int MPI_Init( int *argc, char ***argv )
{
	int mpi_retval;
    int my_rank;
	int numRanks;
	int numNodes; 
	int myNid;
	int rc;
 	mpi_retval = PMPI_Init( argc, argv );
	if ( mpi_retval != MPI_SUCCESS ) {
		return mpi_retval;
	}

    rc =  MPI_Comm_rank( MPI_COMM_WORLD,&my_rank);
	assert( MPI_SUCCESS == rc );

    rc =  MPI_Comm_size( MPI_COMM_WORLD,&numRanks);
	assert( MPI_SUCCESS == rc );

	std::string nidList = createNidList( my_rank, numNodes, myNid );

	if ( nidList.empty() ) {
		return mpi_retval;
	}

	if ( _debug ) {
		printf("PWRRT: numNodes=%d myNid=%d nidlist `%s`\n",
						numNodes,myNid,nidList.c_str());
	}

	std::stringstream ret;
	ret << numNodes;
	setenv( "POWERRT_NUMNODES", &ret.str().c_str()[0], 1 );
	setenv( "POWERRT_NODES_PER_BOARD", "5", 1 );
	setenv( "POWERRT_BOARDS_PER_CAB", "2", 1 );
	setenv( "POWERRT_MACHINE","volta", 1 );

 	assert( ! __data );
	Data* data = __data = new Data;

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

    PyObject* pArgs = PyTuple_New( 6 );
    assert(pArgs);

	std::string routeFile = "/home/mjleven/pwrTest/routeTable.txt"; 
	char* config = getenv("POWERAPI_CONFIG"); 
	if ( ! config ) {
		printf("ERROR: POWERAPI_CONFIG is not set\n");
		exit(-1);
	} 
    PyTuple_SetItem( pArgs, 0, PyInt_FromLong( myNid ) );
    PyTuple_SetItem( pArgs, 1, PyString_FromString( config ) );
    PyTuple_SetItem( pArgs, 2, PyString_FromString( nidList.c_str() ) );
    PyTuple_SetItem( pArgs, 3, PyString_FromString( routeFile.c_str() ) );

	std::string logFile = "";
	std::string object = "plat";
	for ( unsigned i = 0; i < *argc; i++ ) {
		if ( ! strncmp( (*argv)[i], "--pwr_rt_obj=", 13 ) ) {
			object = (*argv)[i] + 13; 
		}	
		if ( ! strncmp( (*argv)[i], "--pwr_rt_file=", 14 ) ) {
			logFile = (*argv)[i] + 14; 
		}	
	}

    PyTuple_SetItem( pArgs, 4, PyString_FromString( object.c_str() ) );
    PyTuple_SetItem( pArgs, 5, PyString_FromString( logFile.c_str() ) );


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

	//if ( 1 ) {
	if ( _debug ) {
		printf("PWRRT: rank=%d launch\n",my_rank);
		for ( unsigned j = 0; j < argv.size(); j++ ) {
			printf("PWRRT:    %s\n",argv[j] );
		}
	}

		int child;
		if ( ( child = fork() ) ) {
			data->child.push_back(child);	
		} else if ( 0 == child )  {
			int rc = execve( argv[0], &argv[0], &envp[0] );
			//int rc = execv( argv[0], &argv[0]  );
			if ( -1 == rc ) {
				fprintf(stderr,"execve failed %s\n",strerror(errno));
				exit(-1);
			}
		} else {
			assert(0);
		}

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
	return mpi_retval;
}

int MPI_Finalize()
{
	Data* data = __data;
	
	if ( data ) {

		for ( unsigned i =0; i < data->child.size(); i++ ) {
			if ( _debug ) {
				printf("PWRRT: kill child %d\n", data->child[i]);
			}
			kill( data->child[i], SIGKILL );
			waitpid( data->child[i],NULL, 0  );
		}
		delete data;
	}

	return PMPI_Finalize();
}

static std::string createNidList(  int my_rank, int& numNodes, int& myNid )
{
	int rc;
	int numRanks;

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

	int nodeid = atoi( &procName[3] );
	if ( _debug ) {
		printf("PWRRT: my_rank=%d procName=%s nodeid=%d\n",
						my_rank, procName.c_str(), nodeid );
	}
	
	std::vector<int> rbuf( numRanks );

	rc = MPI_Allgather( &nodeid, 1, MPI_INT, &rbuf[0], 1, 
											MPI_INT, MPI_COMM_WORLD );
	assert( MPI_SUCCESS == rc ); 

	int launcherRank = -1; 
	for ( int i = 0; i < numRanks; i++ ) {
		if ( rbuf[i] == nodeid && launcherRank == -1 ) {
			launcherRank = i;
			break;
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

			ret << std::setw( procName.size() - pos) << 
									std::setfill('0') << rbuf[i];
		}
	}

	ret << std::setw(1) << "]";

	return ret.str();
}
