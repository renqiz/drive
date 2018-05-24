
VPATH = ../../out/cm256/lib ../../out/ubd/lib

HEADERS = \
	network/Connection.h \
	network/ConnectionFactory.h \
	network/IEndPoint.h \
	network/SocketConnection.h \
	network/SocketConnectionFactory.h \
	network/SocketEndPoint.h \
	network/SocketMessageReceiver.h \
	network/SocketMessageSender.h \
	dsp/CreatePartitionRequest.h \
	dsp/CreatePartitionResponse.h \
	dsp/Instruction.h \
	dsp/InstructionSerializer.h \
	dsp/OpCode.h \
	dsp/Protocol.h \
	dsp/ReadBlockRequest.h \
	dsp/ReadBlockResponse.h \
	dsp/WriteBlockRequest.h \
	dsp/WriteBlockResponse.h \
  Action.h \
	AsyncResult.h \
  BitSet.h \
	Buffer.h \
	BufferedInputStream.h \
	BufferedOutputStream.h \
	DspClient.h \
	IInputStream.h \
	IOutputStream.h \
	LockFreeQueue.h \
  Options.h \
  Partition.h \
	ReferenceCount.h \
  Util.h \
  Volume.h \

SOURCES = \
	network/SocketConnection.cpp \
	network/SocketConnectionFactory.cpp \
	network/SocketEndPoint.cpp \
	network/SocketMessageReceiver.cpp \
	network/SocketMessageSender.cpp \
	dsp/CreatePartitionRequest.cpp \
	dsp/CreatePartitionResponse.cpp \
	dsp/Instruction.cpp \
	dsp/InstructionSerializer.cpp \
	dsp/ReadBlockRequest.cpp \
	dsp/ReadBlockResponse.cpp \
	dsp/WriteBlockRequest.cpp \
	dsp/WriteBlockResponse.cpp \
  Action.cpp \
	AsyncResult.cpp \
  BitSet.cpp \
	Buffer.cpp \
	BufferedInputStream.cpp \
	BufferedOutputStream.cpp \
	DspClient.cpp \
  Main.cpp \
  Options.cpp \
  Partition.cpp \
  Volume.cpp \
  VolumeCell.cpp \
  VolumeColumn.cpp \
  VolumeRow.cpp \
  Util.cpp \

LIBS = \
  ../../ubd/out/lib/ubd.a \
  ../../cm256/out/lib/cm256.a


TARGET = drive
CCFLAGS = -std=c++11 -msse4.1 -I. -I../../ubd/out/inc -I../../cm256/out/inc
LDFLAGS = -lcrypto -pthread

include Cpp.mk
