pynip2p
=======

Python wrapper for the National Instruments peer-to-peer communication library.

This wrapper is based on boost-python and is tested to run on Windows 7.
I used Microsoft Visual Studio Express 2013 to compile (project files included).

The wrapper is made to wrap as closely as possible the functions provided by the NI-P2P C interface. The project is made of a C-to-C++ stage followed by a C++-to-Python stage (I find it more readable in this way).

Architecture
============

Here is a brief list of the functions and other things provided by pynip2p. An extended description can be found in the [NI Peer To Peer Streaming help files](http://zone.ni.com/reference/en-XX/help/372831A-01/), under *Peer To Peer Streaming VIs and Functions*.
The *enum* structures give a convenient way to access stream attributes and events codes.
The functions that are not supposed to return any value will return True if they succeeded, and False otherwise.

**pynip2p**
* (int writerEndpointHandle, int readerEndpointHandle)

*Stream creation/deletion functions*
* bool CreateAndLinkStream(int writerEndpointHandle, int readerEndpointHandle, bool enableStream)
* bool DestroyStream()

*Stream enable/disable functions*
* bool EnableStream()
* bool DisableStream()
* bool FlushAndDisableStream(int flushTimeoutMsec)

*Stream linking/unlinking functions*
* bool LinkStream()
* bool UnlinkStream()

*Stream event handling*
* bool WaitForStreamEvent(int event, int timeoutMsec)

*Property node access*
* int GetAttribute(int attributeKey)

*StreamAttribute enum*
* StreamState
* ReaderNumElementsForReading
* ReaderSizeInElements
* ReaderUnderflow
* WriterNumElementsForWriting
* WriterSizeInElements
* WriterOverflow

*StreamEvent enum*
* FIFOOverflow
* FIFOUnderflow
* StreamEnabled
* StreamDisabled


Usage example
=============

This example shows how to link two devices using pynip2p. It uses [pyniscope](http://github.com/vpaeder/pyniscope) and [pynifpga](http://github.com/vpaeder/pynifpga). It reproduces the example [Working with Interleaved Data using NI-Scope, NI-P2P and NI FlexRIO](http://www.ni.com/example/31370/en/).

	# FPGA settings
	res = "RIO0" # instrument address (resource name, for example RIO0)
	fname = "NiFpga_niScopeEXP2PInterleavedDataFPGA.lvbitx" # Bitfile name
	sig = "SignatureRegister node value found in LVBITX file"
	
	# digitizer settings
	instr_scope = "PXIData" # instrument address
	channels = "0,1" # channel string (comma-separated channel numbers)
	verticalSpan = 0.2 # vertical span in V
	sampleRate = 1e8 # sample rate in sample/sec
	numPoints = 2000 # number of points to be recorded
	inputImpedance = 1e6 # input impedance in Ohm
	timeout = 1500 # acquisition timeout in ms
	
	# peer-to-peer settings
	streamName = "FIFOEndpoint0"
	
	# loads FPGA drivers
	from pynifpga import pynifpga
	fpga = pynifpga()
	
	# loads file into the FPGA
	fpga.Open(fname, sig, res, 0)
	# loads digitizer drivers
	from pyniscope import pyniscope, P2PAttribute
	scope = pyniscope()
	
	# initializes and sets digitizer to the appropriate state
	scope.Initialize(instr_scope, False, False)
	scope.ConfigureVertical(channels, verticalSpan, 0.0, 1, 1.0, True)
	scope.ConfigureChanCharacteristics(channels, inputImpedance, 0.0)
	scope.ConfigureHorizontalTiming(sampleRate, numPoints, 0.0, 1, True)
	
	# sets trigger mode
	scope.ConfigureTriggerSoftware(0.0, 0.0)
	
	# gets scaling coefficients for the values that will be extracted from the FPGA
	coeffs = scope.GetScalingCoefficients(channels)
	
	# gets P2P handle for digitizer
	scopeHandle = scope.GetStreamEndpointHandle(streamName)
	
	# set channels to be streamed
	scope.SetAttributeViString(streamName,P2PAttribute.ChannelsToStream,channels)
	
	# gets P2P write endpoint on FPGA (the FIFO channel numbers are given in the node DmaChannelAllocationList of the .lvbitx file)
	fpgaHandle = fpga.GetPeerToPeerFifoEndpoint(0)
	
	# loads peer-to-peer drivers
	from pynip2p import pynip2p
	p2p = pynip2p()
	
	# sets "P2P enabled" attribute on digitizer to True
	scope.SetAttributeViBoolean(streamName,P2PAttribute.Enabled,True)
	
	# sets the number of points to record (corresponds here to the control #0)
	# the control number can be read from the Offset property of a Register node in the LVBITX file.
	fpga.WriteU32(0,numPoints)
	
	# links digitizer and FPGA together
	p2p.CreateAndLinkStream(scopeHandle, fpgaHandle, True)
	
	# starts digitizing
	scope.InitiateAcquisition()
	scope.SendSWTrigger()
	while scope.AcquisitionStatus()==0:
		pass
	
	# reads data from the FPGA DMA-to-host channels
	data0 = array(fpga.ReadFifoI16(1, numPoints, timeout))*coeffs[0][1] + coeffs[0][0] # scales with scaling coefficients corresponding to the channel
	data1 = array(fpga.ReadFifoI16(2, numPoints, timeout))*coeffs[1][1] + coeffs[1][0] # scales with scaling coefficients corresponding to the channel
	
	# reads actual sample rate and builds time vector
	sr = scope.SampleRate()
	tv = array(range(len(data0)))/sr
	
	# plots results
	from matplotlib import pyplot as pl
	pl.plot(tv,data0,tv,data1)
	pl.show()
	
	# closes instrument connections and cleans up
	p2p.DestroyStream() # destroys P2P stream
	fpga.Close(0) # closes FPGA connection
	scope.Close() # closes digitizer connection
	fpga.Finalize() # cleans up FPGA driver
