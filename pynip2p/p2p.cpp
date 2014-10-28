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

 Wrapper for NI-P2P driver.

 This wrapper translates the NI-P2P C API to C++.

*/

#include <iostream>
#include "p2p.h"
#pragma comment (lib, "nip2p.lib")

NIP2P::NIP2P() {
	streamCreated = false;
	streamLinked = false;
	streamEnabled = false;
	streamHandle = 0;
}

NIP2P::NIP2P(nip2p_tEndpointHandle writerEndpointHandle, nip2p_tEndpointHandle readerEndpointHandle) {
	streamCreated = false;
	streamLinked = false;
	streamEnabled = false;
	streamHandle = 0;
	CreateAndLinkStream(writerEndpointHandle, readerEndpointHandle, 0);
}

NIP2P::~NIP2P() {
	DestroyStream();
}

bool NIP2P::HandleStatus(uint32_t status) {
	if (status < 0) {
		std::cout << "[ERROR] : " << status << std::endl;
		DestroyStream();
		return false;
	}
	else if (status > 0) {
		std::cout << "[WARNING] : " << status << std::endl;
	}
	return true;
}

bool NIP2P::CreateAndLinkStream(nip2p_tEndpointHandle writerEndpointHandle, nip2p_tEndpointHandle readerEndpointHandle, uint8_t enableStream) {
	bool status = false;
	if (!streamCreated) status = HandleStatus(nip2pCreateAndLinkStream(writerEndpointHandle, readerEndpointHandle, enableStream, &streamHandle));
	if (status) {
		streamCreated = true;
		streamLinked = (enableStream != 0);
		streamEnabled = (enableStream != 0);
	}
	return status;
}

bool NIP2P::DestroyStream() {
	bool status = false;
	if (streamCreated) {
		if (streamEnabled) status = DisableStream();
		if (status) streamEnabled = false;
		if (streamLinked) status = UnlinkStream();
		if (status) streamLinked = false;
		status = HandleStatus(nip2pDestroyStream(streamHandle));
		if (status) streamCreated = false;
	}
	return status;
}

bool NIP2P::EnableStream() {
	bool status = false;
	if (streamCreated && !streamEnabled) {
		if (!streamLinked) status = LinkStream();
		if (streamLinked) status = HandleStatus(nip2pEnableStream(streamHandle));
		if (status) streamCreated = true;
	}
	return status;
}

bool NIP2P::DisableStream() {
	bool status = false;
	if (streamCreated && streamLinked && streamEnabled) status = HandleStatus(nip2pDisableStream(streamHandle));
	if (status) streamEnabled = false;
	return status;
}

bool NIP2P::FlushAndDisableStream(int32_t flushTimeoutMsec) {
	bool status = false;
	uint8_t flushTimedOut;
	if (streamCreated && streamLinked && streamEnabled) status = HandleStatus(nip2pFlushAndDisableStream(streamHandle,flushTimeoutMsec,&flushTimedOut));
	if (status) streamEnabled = false;
	if (flushTimedOut != 0) status = false;
	return status;
}

bool NIP2P::WaitForStreamEvent(nip2p_tStreamEvent evt, int32_t timeoutMsec) {
	bool status = false;
	if (streamCreated) status = HandleStatus(nip2pWaitForStreamEvent(streamHandle, evt, timeoutMsec));
	return status;
}

bool NIP2P::LinkStream() {
	bool status = false;
	if (streamCreated && !streamLinked) status = HandleStatus(nip2pLinkStream(streamHandle));
	if (status) streamLinked = true;
	return status;
}

bool NIP2P::UnlinkStream() {
	bool status = false;
	if (streamCreated && streamLinked) {
		if (streamEnabled) status = DisableStream();
		if (status) status = HandleStatus(nip2pLinkStream(streamHandle));
		if (status) streamLinked = false;
	}
	return status;
}

bool NIP2P::GetAttribute(nip2p_tStreamAttribute attributeKey, void* value) {
	bool status = false;
	if (streamCreated) status = HandleStatus(nip2pGetAttribute(streamHandle, attributeKey, value));
	return status;
}
