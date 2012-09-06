/*
 * 					XNET
 * 					 by Biagio Festa
 * 					 2012
 *
 * 			INCLUDE MACRO
 * 		Define XNET_UNSAFE_INCLUSION to 'see' all files of library.
 * 		Define XNET_INCLUDE_CLIENT_PACK to 'see' all files of library about Client connection classes
 * 		Define XNET_INCLUDE_SERVER_PACK to 'see' all files of library about Server connection classes
 */

#ifndef XNET_HPP_
#define XNET_HPP_

#ifdef XNET_UNSAFE_INCLUSION
#include <XNet/server/ConServer.hpp>
#include <XNet/client/ConClient.hpp>
#else
#ifdef XNET_INCLUDE_CLIENT_PACK
#include <XNet/client/ConClient.hpp>
#endif
#ifdef XNET_INCLUDE_SERVER_PACK
#include <XNet/server/ConServer.hpp>
#endif
#endif

#endif /* XNET_HPP_ */
