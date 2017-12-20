bin_PROGRAMS = logServerHost test_basic test_with_log_server
#test_log_server
#test_tcp

# test without log server
test_basic_LDADD = ../libTraceDebug.a  -lstdc++ -lrt
test_basic_CXXFLAGS = -g
test_basic_SOURCES = test.cpp

# test with log server
test_with_log_server_LDADD = ../libTraceDebugWithTcpServer.a ../../Framework/utils/libUtil.a ../libTraceDebugWithTcpServer.a ../../Framework/delayServer/libDelayServer.a -lstdc++ -lrt
test_with_log_server_CXXFLAGS = -g -pthread -DENABLE_LOG_SERVER
test_with_log_server_SOURCES = test.cpp

# unit test of tcp server (one short reconnection)
#test_tcp_LDADD    = ../../Framework/utils/libUtil.a ../libTraceDebug.a ../../Framework/utils/libUtil.a -lstdc++
#test_tcp_CXXFLAGS = -g -pthread
#test_tcp_SOURCES  = test_tcp.cpp ../TcpServer.cpp ../TcpServer.h

# unit test of tcp server / multiple clients
logServerHost_LDADD    = ../../Framework/utils/libUtil.a ../libTraceDebugWithTcpServer.a ../../Framework/utils/libUtil.a -lstdc++ -lrt
logServerHost_CXXFLAGS = -g -pthread
logServerHost_SOURCES  = logServerHost.cpp

# unit test of tcp server thread (allow reconnection)
#test_log_server_LDADD    = ../../Framework/utils/libUtil.a  ../libTraceDebug.a ../../Framework/delayServer/libDelayServer.a
#test_log_server_CXXFLAGS = -g -pthread
#test_log_server_SOURCES  = test_log_server.cpp  ../TcpServer.cpp ../TcpServer.h ../LogServerThread.cpp ../LogServerThread.h

.PHONY: test

#test: logServerHost test_with_log_server
#../Framework/logServerHost &
#	./test_with_log_server

test: test_basic
	./test_basic

#test: test_log_server
#	./test_log_server

#test: test_with_log_server
#	./test_with_log_server

clean_:
	-rm -f *~
	-rm -f TAGS
