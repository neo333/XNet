/*
 * lib_set.hpp
 *
 *  Created on: 31/ago/2012
 *      Author: neo
 */

#ifndef LIB_SET_HPP_
#define LIB_SET_HPP_

#ifdef __DLL_BUILD__
	#ifdef WIN32
		#define XNET_DLL __declspec(dllexport)
	#else
		#define XNET_DLL
	#endif
#else
	#ifdef WIN32
		#define XNET_DLL __declspec(dllimport)
	#else
		#define XNET_DLL
	#endif
#endif

#define XNET_INLINE inline




#endif /* LIB_SET_HPP_ */
