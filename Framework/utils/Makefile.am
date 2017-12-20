
## default to four parallel runs
MAKEFLAGS = -j 4

noinst_LIBRARIES = libUtil.a  libUtil_Lite.a

bin_PROGRAMS = testsem testsem_posix testsem_both testbithacks

LDADD = libUtil.a ../log/libTraceDebug.a -lrt

AM_LDFLAGS =   -pthread

libUtil_a_SOURCES =  	\
  ThresholdWithDelay.h ThresholdWithDelay.cpp \
	JitterMonitor.h 	JitterMonitor.cpp \
	filter.cpp Sem.cpp SemPosix.cpp \
	filter.h Sem.h SemI.h SemPosix.h \
	SemCollection.cpp SemCollection.h \
	SemOp.cpp SemOp.h \
	SemTable.cpp SemTable.h \
	sem_helper.cpp sem_helper.h \
	thread.cpp thread.h \
	Chrono.cpp Chrono.h \
	Time.cpp Time.h \
	SendMail.h \
	TimeManager.cpp TimeManager.h \
	SHA256.cpp SHA256.h \
    AesAlgo.cpp AesAlgo.h \
    sha2.c sha2.h \
    PingUtils.cpp PingUtils.h \
	FormatUtils.cpp FormatUtils.h \
	FileProcessing.cpp FileProcessing.h \
	Curl_Helper.cpp Curl_Helper.h \
	Observable.h \
	Observer.h \
  Uptime.h Uptime.cpp

libUtil_Lite_a_SOURCES=$(libUtil_a_SOURCES)
libUtil_Lite_a_CPPFLAGS = -DWWWLITE

testsem_posix_SOURCES  = testSemPosix.cpp stub.h stub.cpp
testsem_SOURCES        = testSem.cpp  stub.h stub.cpp
testsem_both_SOURCES   = testSemBoth.cpp stub.h stub.cpp

testbithacks_SOURCES = bithacks-test.c bithacks.h

.PHONY:test

#./testsem;
#./testsem_posix;
test:testsem
	valgrind --leak-check=full --show-reachable=yes  ./testsem;

#./testbithacks;

testSharedSem: testsem_both
	./testsem_both 1& ./testsem_both 2
