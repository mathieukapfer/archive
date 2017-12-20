# Create libs
add_library(TraceDebug  STATIC 	TraceDebug)
add_library(TraceDebugWithTcpServer STATIC
        TraceDebug
        TcpServer
        LogServerThread)
 
set_target_properties(TraceDebugWithTcpServer PROPERTIES COMPILE_FLAGS "-DENABLE_LOG_SERVER")

add_subdirectory(test)