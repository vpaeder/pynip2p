/*
	Copyright (c) 2014 Vincent Paeder
	
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*

 Python wrapper for NI-P2P driver.

 This wrapper translates the NI-P2P C API to python.

*/

#ifndef _NI_P2P_WRAP_H_
#define _NI_P2P_WRAP_H_

#include "nip2p.h"

class NIP2P {
protected:
	nip2p_tStreamHandle streamHandle;
	bool streamCreated;
	bool streamLinked;
	bool streamEnabled;

	bool HandleStatus(uint32_t status);
public:
	NIP2P();
	NIP2P(nip2p_tEndpointHandle writerEndpointHandle, nip2p_tEndpointHandle readerEndpointHandle);
	~NIP2P();

	bool CreateAndLinkStream(nip2p_tEndpointHandle writerEndpointHandle, nip2p_tEndpointHandle readerEndpointHandle, uint8_t enableStream);
	bool DestroyStream();

	bool EnableStream();
	bool DisableStream();
	bool FlushAndDisableStream(int32_t flushTimeoutMsec);

	bool WaitForStreamEvent(nip2p_tStreamEvent evt, int32_t timeoutMsec);

	bool LinkStream();
	bool UnlinkStream();

	bool GetAttribute(nip2p_tStreamAttribute attributeKey, void* value);

};

#endif
