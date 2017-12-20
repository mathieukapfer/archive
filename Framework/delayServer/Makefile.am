## default to four parallel runs
MAKEFLAGS = -j 4

noinst_LIBRARIES = libDelayServer.a

libDelayServer_a_SOURCES = \
	DelayServer.cpp DelayServer.h \
	DelayServerPrio.cpp DelayServerPrio.h \
	TimerInfoList.cpp TimerInfoList.h \
	TimerDb.cpp TimerDb.h \
	TimerEventI.h TimerEventI.cpp

.PHONY : test

test: libDelayServer.a
	cd test; make test

test0: libDelayServer.a
	cd test; make test0
