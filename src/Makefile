
VPATH = ../../out/cm256/lib ../../out/ubd/lib

HEADERS = \
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
	BufferedInputStream.h \
	BufferedOutputStream.h \
	IInputStream.h \
	IOutputStream.h \
  Options.h \
  Partition.h \
  Util.h \
  Volume.h \

SOURCES = \
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
	BufferedInputStream.cpp \
	BufferedOutputStream.cpp \
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
LDFLAGS = -lcrypto

include Cpp.mk
