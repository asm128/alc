#include "alc_platform_globals.h"

#include "llc_log.h"
#include "llc_functional.h"
#include "llc_array_circular.h"

#ifndef ALC_STATIC_BUS_H
#define ALC_STATIC_BUS_H

namespace alc
{
	template<u1_t _nIn, u1_t _nOut>
	struct SStaticBusState {
		u0_t			InputBuffer		[_nIn ]	= {};
		u0_t			OutputBuffer	[_nOut]	= {};

		//::llc::acircu8<_nIn >	Input	= {};
		//::llc::acircu8<_nOut>	Output	= {};
		u1_t		InputBufferEnd 			= 0;
		u1_t		OutputBufferEnd 		= 0;

		vltl	bool	Receiving				= {};
		vltl	bool	Online					= {};

		::llc::error_t	ReadWire				(int byteCount, const ::llc::function<int()> & wireAvailable, const ::llc::function<char()> & wireReadByte) {
			u1_t		iByte	= InputBufferEnd = 0;
			for(const u1_t stop = ::llc::clamped((u1_t)byteCount, (u1_t)0, (u1_t)::llc::size(InputBuffer)); InputBufferEnd < stop; ++iByte, ++InputBufferEnd) {
				ree_if(false == wireAvailable(), LLC_FMT_U32_LT_U32, iByte, byteCount);
				InputBuffer[InputBufferEnd] = wireReadByte();
			}
			return iByte;
		}
	};
} // namespace 

#endif // ALC_STATIC_BUS_H

// #include "gpk_log.h"
// #include "gpk_functional.h"
// #include "gpk_array_circular.h"

// #ifndef ALC_STATIC_BUS_H
// #define ALC_STATIC_BUS_H

// namespace gpk
// {
// 	tplt<tpnm _tPOD = u0_t, u1_t _nIn = 32, u1_t _nOut = 32>
// 	struct SStaticBusState {
// 		typedef _tPOD T;
// 		::gpk::acir<T, _nIn >	Input			= {};

// 		vltl	u0_t			Receiving		= 0;
// 		vltl	bool			Online			= {};
// 	};
// } // namespace 

// #endif // ALC_STATIC_BUS_H
