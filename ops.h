
#ifndef _PWR_ATTRIBUTE_H
#define _PWR_ATTRIBUTE_H

#include <vector>

static void fpSum( int num, void* _out, void* _in )
{
    double* out = (double*) _out;
    double* in  = (double*) _in;
    *out = in[0];

    //printf("%s() i=%d val=%f\n",__func__, 0, in[0]);
    for ( int i = 1; i < num ;i++  ) {
        //printf("%s() i=%d val=%f\n",__func__, i, in[i]);
        *out += in[i];
    }
}

static void fpSum2( void* out, const std::vector<void*>& in  )
{
	//fprintf(stderr,"%s() %lu\n",__func__,in.size());
    *((double*) out) = *((double*) in[0]);

    //fprintf(stderr,"%s() i=%d val=%f\n",__func__, 0, *((double*) out));

    for ( unsigned int i=1; i < in.size(); i++ ) {
        //fprintf(stderr,"%s() i=%d val=%f\n",__func__, i, *((double*) in[i]));
        *((double*) out) += *((double*) in[i]);
    }
}

#endif
