#ifndef _SERIALIZE_H
#define _SERIALIZE_H

#include <assert.h>
#include <vector>
#include <stdio.h>
#include <stdint.h>

struct SerialBuf {

	SerialBuf() {}
	SerialBuf( size_t length ) { buf.resize(length); }

	SerialBuf& operator<<( const std::string& str ) {
		size_t len = str.length();
        for ( unsigned int i = 0; i < len ; i++ ) {
            buf.push_back( str[i] );
        }

		*this << len;
		
		return *this;
	}
	void print( ) {
		for ( unsigned i = 0; i < buf.size(); i++ ) {
			fprintf(stderr,"%02x ", buf[i]);
		}
	}

	const SerialBuf& operator>>( std::string& str ) {
		size_t len;
		*this >> len;

		str.resize(len);
        for ( unsigned int i = len; i > 0 ; i-- ) {
			str[ i - 1 ] = buf.back();
			buf.pop_back();
        }
		return *this;
	}

	template<typename T> SerialBuf& operator<<( const std::vector<T>& vec ) {
		size_t len = vec.size();

        for ( unsigned int i = 0; i < len ; i++ ) {
        	*this << vec[i];
        }
		*this << len;
		return *this;
	}

	template<typename T> const SerialBuf& operator>>( std::vector<T>& vec ) {
		size_t len;
		*this >> len;

		vec.resize(len);
        for ( unsigned int i = len; i > 0 ; i-- ) {
			*this >> vec[i-1];
        }
		return *this;
	}

    template<typename T> SerialBuf& operator<<( const T t ) {
		
		assert( sizeof( T ) <= sizeof(uint64_t) );

		uint64_t tmp = (uint64_t) t;

		out(tmp); 
        return *this;
	}

    SerialBuf& operator<<( const double t ) {

		uint64_t tmp = *(uint64_t*) &t;
		out(tmp);
        return *this;
	}

	void out( uint64_t tmp ) { 
        for ( unsigned int i = 0; i < sizeof(tmp) ; i++ ) {
			unsigned char c = tmp >> (8 * i ) & 0xff;
            buf.push_back( c );
        }
    }

    template<typename T> SerialBuf& operator>>( T& t ) {
		assert( sizeof( T ) <= sizeof(uint64_t) );
		t =	(T) in();
		return *this; 
	}	

    SerialBuf& operator>>( double& t ) {
				
		uint64_t tmp = in();
		t = *(double*) &tmp;			
        return *this;
	}

	uint64_t in() { 
        uint64_t tmp = 0;
        for ( unsigned int i = sizeof(uint64_t); i > 0 ; i-- ) {
			uint64_t c = buf.back();
            buf.pop_back();
			tmp |= (c << (8 * (i-1)) );
        }
		return tmp;
    }

    std::vector<unsigned char > buf;
	size_t length() { return buf.size(); } 
	void* addr() { return &buf[0]; }
};

struct Serialize {
    virtual void serialize_in( SerialBuf& ) = 0;
    virtual void serialize_out( SerialBuf& ) = 0;
};

#endif
