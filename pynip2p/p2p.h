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
