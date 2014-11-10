#pragma once

#include <vector>
#include <map>

/*
 * Templated sample processing. This is a flexible way to sample and process an 
 * arbitrary data set with minimal code. The SampleT<T> stores a vector of type T.
 * It also keeps a map of processes which return type Y. The second part of the map 
 * is a std::function which returns Y. The first part of the map is a size_t, 
 * which represents your ID for the function. This is handy for creating enumerators
 * to organize your processes.
 */

namespace sampling {

//////////////////////////////////////////////////////////////////////////////////////////////

class Exception : public std::exception {
{
public:
	Exception();
	virtual ~Exception() throw() {}
	
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
		sprintf_s( mMessage, "Process not found: %i", index );
	}
};

class ExcProcUndefined : public Exception 
{
public:
	ExcProcUndefined( size_t index ) throw()
	{
		sprintf_s( mMessage, "Process is undefined: %i", index );
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Y>
class SamplerT
{
protected:
	size_t									mNumSamples;
	std::vector<T>							mSamples;
	std::map<size_t, std::function<Y()> >	mProcessMap;

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
