## default to four parallel runs
MAKEFLAGS = -j 4

noinst_LIBRARIES = libTraceDebug.a libTraceDebugWithTcpServer.a libTraceDebugWebserver.a

libTraceDebug_a_SOURCES  = TraceDebug.cpp TraceDebug.h Logger.cpp Logger.h ApplicationLevelTab.cpp ApplicationLevelTab.h FileLevelTab.cpp FileLevelTab.h tracedebug.h tracedebug.cpp LogFormater.h LogFormater.cpp LogMacro.h
libTraceDebugWithTcpServer_a_SOURCES  = $(libTraceDebug_a_SOURCES) TcpServer.cpp TcpServer.h LogServerThread.cpp LogServerThread.h TcpServerMultiClients.cpp TcpServerMultiClients.h TcpClient.cpp  TcpClient.h TcpWrite.cpp TcpWrite.h TcpWriteI.h HostConnection.h HostConnection.cpp
libTraceDebugWithTcpServer_a_CXXFLAGS = -DENABLE_LOG_SERVER

libTraceDebugWebserver_a_SOURCES  = $(libTraceDebugWithTcpServer_a_SOURCES)
libTraceDebugWebserver_a_CXXFLAGS = -DENABLE_LOG_SERVER -DWEBSERVER

.PHONY: test

test: libTraceDebugWithTcpServer.a libTraceDebug.a
	cd test; make test

clean_:
	-rm -f *~
	-rm -f TAGS
