/*
 *
 * Copyright (c) 2016, Ban the Rewind
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ban the Rewind nor the names of its
 * contributors may be used to endorse or promote products
 * derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * Sampling is a templated data processing library. This is a flexible way to
 * sample and process an arbitrary data set with minimal code. The SampleT<T> 
 * stores a vector of type T. It also keeps a map of processes which return type Y. 
 * The second part of the map is a std::function which returns Y. The first part of 
 * the map is a size_t, which represents your ID for the function. This is handy 
 * for creating enumerators to organize your processes.
 */

#pragma once

#include <vector>
#include <map>

namespace sampling {

//////////////////////////////////////////////////////////////////////////////////////////////

class Exception : public std::exception
{
public:
	Exception()
	{
	}
	
	virtual ~Exception() throw()
	{
	}
	
	inline const char*	what() const throw()
	{ 
		return mMessage; 
	}
protected:
	char mMessage[ 2048 ];
};

class ExcProcNotFound : public Exception 
{
public:
	ExcProcNotFound( size_t index ) throw()
	{
		printf( mMessage, "Process not found: %i", index );
	}
};

class ExcProcUndefined : public Exception 
{
public:
	ExcProcUndefined( size_t index ) throw()
	{
		printf( mMessage, "Process is undefined: %i", index );
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Y>
class SamplerT
{
protected:
	size_t									mNumSamples;
	std::map<size_t, std::function<Y()> >	mProcessMap;
	std::vector<T>							mSamples;

	inline void fit()
	{
		if ( mNumSamples < 1 ) {
			mNumSamples = 1;
		}
		while ( mSamples.size() > mNumSamples ) {
			mSamples.erase( mSamples.begin() );
		}
		if ( mSamples.size() < mNumSamples ) {
			size_t count = mNumSamples - mSamples.size();
			mSamples.insert( mSamples.begin(), count, T() );
		}
	}
public:
	SamplerT( size_t numSamples = 2 )
		: mNumSamples( numSamples )
	{
	}
	
	SamplerT( const SamplerT& rhs )
	{
		*this = rhs;
	}
	
	SamplerT& operator=( const SamplerT& rhs )
	{
		mNumSamples	= rhs.mNumSamples;
		mProcessMap	= rhs.mProcessMap;
		mSamples	= rhs.mSamples;
		return *this;
	}

	inline SamplerT& process( size_t index, const std::function<Y()>& func )
	{
		setProcess( index, func );
		return *this;
	}

	inline void	clearProcesses()
	{
		mProcessMap.clear();
	}

	inline void	eraseProcess( size_t index )
	{
		if ( mProcessMap.find( index ) != mProcessMap.end() ) {
			mProcessMap.erase( index );
		}
		throw ExcProcNotFound( index );
	}

	inline std::function<Y()>& getProcess( size_t index )
	{
		if ( mProcessMap.find( index ) != mProcessMap.end() ) {
			return mProcessMap.at( index );
		}
		throw ExcProcNotFound( index );
	}

	inline const std::function<Y()>& getProcess( size_t index ) const
	{
		if ( mProcessMap.find( index ) != mProcessMap.end() ) {
			return mProcessMap.at( index );
		}
		throw ExcProcNotFound( index );
	}

	inline void setProcess( size_t index, const std::function<Y()>& func )
	{
		mProcessMap[ index ] = func;
	}

	inline std::map<size_t, std::function<Y()> >& getProcessMap()
	{
		return mProcessMap;
	}

	inline const std::map<size_t, std::function<Y()> >&	getProcessMap() const
	{
		return mProcessMap;
	}

	inline Y runProcess( size_t index )
	{
		if ( mProcessMap.find( index ) != mProcessMap.end() ) {
			if ( mProcessMap.at( index ) != nullptr ) {
				return mProcessMap.at( index )();
			}
			throw ExcProcUndefined( index );
		}
		throw ExcProcNotFound( index );
	}

	inline size_t getNumSamples() const
	{
		return mNumSamples;
	}
	
	inline void setNumSamples( size_t numSamples )
	{
		mNumSamples = numSamples;
		fit();
	}

	inline void clearSamples()
	{
		mSamples.clear();
	}

	inline void eraseSample( size_t index )
	{
		if ( mSamples.size() > index ) {
			mSamples.erase( mSamples.begin() + index );
		}
	}

	std::vector<T>& getSamples()
	{
		return mSamples;
	}

	const std::vector<T>& getSamples() const
	{
		return mSamples;
	}

	inline void insertSample( size_t index, const T& v )
	{
		mSamples.insert( mSamples.begin() + index, v );
		fit();
	}

	inline void	pushBack( const T& v )
	{
		mSamples.push_back( v );
		fit();
	}
};

}
