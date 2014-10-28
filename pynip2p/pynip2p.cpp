// include boost headers
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/list.hpp>
using namespace boost::python;

#include "p2p.h"

#include <iostream>

class pynip2p {
private:
	NIP2P stream;
public:
	pynip2p();
	pynip2p(int writerEndpointHandle, int readerEndpointHandle);
	~pynip2p();

	bool CreateAndLinkStream(int writerEndpointHandle, int readerEndpointHandle, bool enableStream);
	bool DestroyStream();

	bool EnableStream();
	bool DisableStream();
	bool FlushAndDisableStream(int flushTimeoutMsec);

	bool WaitForStreamEvent(int evt, int timeoutMsec);

	bool LinkStream();
	bool UnlinkStream();

	int GetAttribute(int attributeKey);

};

pynip2p::pynip2p() {}
pynip2p::pynip2p(int writerEndpointHandle, int readerEndpointHandle) { CreateAndLinkStream(writerEndpointHandle, readerEndpointHandle, false); }
pynip2p::~pynip2p() { DestroyStream(); }

bool pynip2p::CreateAndLinkStream(int writerEndpointHandle, int readerEndpointHandle, bool enableStream) {
	return stream.CreateAndLinkStream((nip2p_tEndpointHandle)writerEndpointHandle, (nip2p_tEndpointHandle)readerEndpointHandle, (uint8_t)enableStream);
}

bool pynip2p::DestroyStream() { return stream.DestroyStream(); }

bool pynip2p::EnableStream() { return stream.EnableStream(); }
bool pynip2p::DisableStream() { return stream.DisableStream(); }
bool pynip2p::FlushAndDisableStream(int flushTimeoutMsec) { return stream.FlushAndDisableStream((int32_t) flushTimeoutMsec); }

bool pynip2p::WaitForStreamEvent(int evt, int timeoutMsec) {
	switch (evt) {
	case 0:
		return stream.WaitForStreamEvent(nip2p_kStreamEventFIFOOverflow, (int32_t)timeoutMsec);
	case 1:
		return stream.WaitForStreamEvent(nip2p_kStreamEventFIFOUnderflow, (int32_t)timeoutMsec);
	case 2:
		return stream.WaitForStreamEvent(nip2p_kStreamEventStreamEnabled, (int32_t)timeoutMsec);
	case 3:
		return stream.WaitForStreamEvent(nip2p_kStreamEventStreamDisabled, (int32_t)timeoutMsec);
	default:
		return false;
	}
}

bool pynip2p::LinkStream() { return stream.LinkStream(); }
bool pynip2p::UnlinkStream() { return stream.UnlinkStream(); }

int pynip2p::GetAttribute(int attributeKey) {
	if (attributeKey == 0) {
		nip2p_tStreamState value;
		if (stream.GetAttribute(nip2p_kStreamAttributeStreamState, &value)) return (int)value;
	}
	else if (attributeKey == 1) {
		uint32_t value;
		if (stream.GetAttribute(nip2p_kStreamAttributeReaderNumElementsForReading, &value)) return (int)value;
	}
	else if (attributeKey == 2) {
		uint32_t value;
		if (stream.GetAttribute(nip2p_kStreamAttributeReaderSizeInElements, &value)) return (int)value;
	}
	else if (attributeKey == 3) {
		uint8_t value;
		if (stream.GetAttribute(nip2p_kStreamAttributeReaderUnderflow, &value)) return (int)value;
	}
	else if (attributeKey == 4) {
		uint32_t value;
		if (stream.GetAttribute(nip2p_kStreamAttributeWriterNumElementsForWriting, &value)) return (int)value;
	}
	else if (attributeKey == 5) {
		uint32_t value;
		if (stream.GetAttribute(nip2p_kStreamAttributeWriterSizeInElements, &value)) return (int)value;
	}
	else if (attributeKey == 6) {
		uint8_t value;
		if (stream.GetAttribute(nip2p_kStreamAttributeWriterOverflow, &value)) return (int)value;
	}
	return 0;
}

int main(int argc, char* argv) {
	return 0;
}

typedef enum {
	StreamState = nip2p_kStreamAttributeStreamState,
	ReaderNumElementsForReading = nip2p_kStreamAttributeReaderNumElementsForReading,
	ReaderSizeInElements = nip2p_kStreamAttributeReaderSizeInElements,
	ReaderUnderflow = nip2p_kStreamAttributeReaderUnderflow,
	WriterNumElementsForWriting = nip2p_kStreamAttributeWriterNumElementsForWriting,
	WriterSizeInElements = nip2p_kStreamAttributeWriterSizeInElements,
	WriterOverflow = nip2p_kStreamAttributeWriterOverflow
} StreamAttribute;

typedef enum {
	FIFOOverflow = nip2p_kStreamEventFIFOOverflow,
	FIFOUnderflow = nip2p_kStreamEventFIFOUnderflow,
	StreamEnabled = nip2p_kStreamEventStreamEnabled,
	StreamDisabled = nip2p_kStreamEventStreamDisabled
} StreamEvent;

BOOST_PYTHON_MODULE(pynip2p)
{
	class_<pynip2p>("pynip2p", init<>())
		.def("CreateAndLinkStream", &pynip2p::CreateAndLinkStream)
		.def("DestroyStream", &pynip2p::DestroyStream)
		.def("EnableStream", &pynip2p::EnableStream)
		.def("DisableStream", &pynip2p::DisableStream)
		.def("FlushAndDisableStream", &pynip2p::FlushAndDisableStream)
		.def("WaitForStreamEvent", &pynip2p::WaitForStreamEvent)
		.def("LinkStream", &pynip2p::LinkStream)
		.def("UnlinkStream", &pynip2p::UnlinkStream)
		.def("GetAttribute", &pynip2p::GetAttribute)
		;
	
	enum_<StreamAttribute>("StreamAttribute")
		.value("StreamState", StreamState)
		.value("ReaderNumElementsForReading", ReaderNumElementsForReading)
		.value("ReaderSizeInElements", ReaderSizeInElements)
		.value("ReaderUnderflow", ReaderUnderflow)
		.value("WriterNumElementsForWriting", WriterNumElementsForWriting)
		.value("WriterSizeInElements", WriterSizeInElements)
		.value("WriterOverflow", WriterOverflow);
	
	enum_<StreamEvent>("StreamEvent")
		.value("FIFOOverflow", FIFOOverflow)
		.value("FIFOUnderflow", FIFOUnderflow)
		.value("StreamEnabled", StreamEnabled)
		.value("StreamDisabled", StreamDisabled);
}
