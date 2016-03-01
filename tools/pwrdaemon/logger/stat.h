
#ifndef _STAT_H
#define _STAT_H

#include <sstream>
#include <iomanip>
#include "work.h" 

#include <util.h>
#include "./util.h"

namespace PWR_Logger {

class Stat : public Work {
  public:
	Stat( PWR_Cntxt ctx, std::string objName, std::string attrs ) {

		printf("%s %s\n",objName.c_str(), attrs.c_str());
		PWR_Obj obj;
		int rc;
   		rc = PWR_CntxtGetObjByName( ctx, objName.c_str(), &obj );
       	assert( rc == PWR_RET_SUCCESS );


		size_t pos = 0;
	 	pos = attrs.find_first_of( ',', 0 );

		m_attr =  getAttr( attrs.substr(0,pos) );

 	 	m_attrStat = getAttrStat( attrs.substr(pos+1)); 

		rc = PWR_ObjCreateStat( obj, m_attr, m_attrStat, &m_stat ); 
       	assert( rc == PWR_RET_SUCCESS );
	}
    int work( FILE* );

  private:
	PWR_Stat 	 m_stat;
	PWR_AttrStat m_attrStat;
	PWR_AttrName m_attr;
};

static std::string getTimeStr( PWR_TimePeriod time );

static inline PWR_Time foo ( struct timeval* ts )
{
	PWR_Time ret = ts->tv_sec * 1000000000;
	ret += ts->tv_usec * 1000;
	return ret;
}


int Stat::work( FILE* fp )
{
	int rc;

	rc = PWR_StatStart( m_stat );

    if( rc != PWR_RET_SUCCESS ) {
		fprintf(fp,"ERROR: PWR_Start( `) failed," " retval=%d\n", rc );
		fflush(fp);
		return -1;
    }

    while( 1 ) {
		PWR_TimePeriod timeInfo;
   		struct timeval tv;

        sleep(1);
		
		gettimeofday(&tv, NULL);

    	PWR_Time stop = foo(&tv); 
		
		timeInfo.start = stop - 1000000000;
		timeInfo.stop = stop;

		double value;
		
		rc = PWR_StatGetValue( m_stat, &value, &timeInfo );
    	if ( rc != PWR_RET_SUCCESS ) {
			fprintf(fp,"ERROR: PWR_StatGetValue( `) failed," " retval=%d\n", 
										rc );
			fflush(fp);
			return -1;
		}

     	fprintf(fp,"Logger:  %-10s %-10s  %-15.2f %-10s %s\n",
				attrStatToString( m_attrStat), 
				attrNameToString( m_attr), 
				value,
                attrUnitsToString( m_attr),
                getTimeStr( timeInfo ).c_str() );



        fflush(fp);
    }

	return 0;
}

std::string getTimeStr( PWR_TimePeriod time ) {
    std::stringstream ret;

    ret << std::setprecision( 6 ) << std::fixed;
    ret << "start " << (double) time.start/1000000000.0 << " sec, " << 
            "stop " << (double) time.stop/1000000000.0 << " sec";

    if ( PWR_TIME_UNINIT != time.instant ) {
        ret << ", instant " << (double) time.instant/1000000000.0 << " sec";
    }
    return ret.str();

} 

}

#endif
