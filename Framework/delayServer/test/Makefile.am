bin_PROGRAMS = StressTest TimerTest DelayServerTest DelayServerTestJitter TimerDbTest

LDADD = ../libDelayServer.a ../../Framework/log/libTraceDebug.a ../../Framework/utils/libUtil.a
LDFLAGS = -lrt -pthread

StressTest_SOURCES        = StressTest.cpp
TimerTest_SOURCES        = TimerTest.cpp
DelayServerTest_SOURCES  = DelayServerTest.cpp
DelayServerTestJitter_SOURCES  = DelayServerTestJitter.cpp
TimerDbTest_SOURCES = TimerDbTest.cpp

test: StressTest
	./StressTest

test1: DelayServerTestJitter
	./DelayServerTest

# test0: DelayServerTest0
# 	valgrind --leak-check=full --show-reachable=yes ./DelayServerTest0
