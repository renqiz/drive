
VPATH = ../../out/cm256/lib ../../out/ubd/lib

HEADERS = \
	network/Client.h \
	network/Connection.h \
	network/FileClient.h \
	network/FileConnection.h \
	network/FileEndPoint.h \
	network/IEndPoint.h \
	network/Server.h \
	network/FileServer.h \
	protocol/CreatePartitionRequest.h \
	protocol/CreatePartitionResponse.h \
	protocol/Instruction.h \
	protocol/InstructionSerializer.h \
	protocol/OpCode.h \
	protocol/Protocol.h \
	protocol/ReadBlockRequest.h \
	protocol/ReadBlockResponse.h \
	protocol/WriteBlockRequest.h \
	protocol/WriteBlockResponse.h \
  Action.h \
  BitSet.h \
	Buffer.h \
	BufferedInputStream.h \
	BufferedOutputStream.h \
	EventLoop.h\
	IInputStream.h \
	IOutputStream.h \
	LockFreeQueue.h \
  Options.h \
  Partition.h \
	Thread.h \
  Util.h \
  Volume.h \

SOURCES = \
	network/Client.cpp \
	network/FileClient.cpp \
	network/FileConnection.cpp \
	network/FileEndPoint.cpp \
	network/FileServer.cpp \
	protocol/CreatePartitionRequest.cpp \
	protocol/CreatePartitionResponse.cpp \
	protocol/Instruction.cpp \
	protocol/InstructionSerializer.cpp \
	protocol/ReadBlockRequest.cpp \
	protocol/ReadBlockResponse.cpp \
	protocol/WriteBlockRequest.cpp \
	protocol/WriteBlockResponse.cpp \
  Action.cpp \
  BitSet.cpp \
	Buffer.cpp \
	BufferedInputStream.cpp \
	BufferedOutputStream.cpp \
	EventLoop.cpp \
  Main.cpp \
  Options.cpp \
  Partition.cpp \
	Thread.cpp \
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
