# Add TESTU
add_executable(test_basic test.cpp)
add_executable(test_with_log_server test.cpp)
add_executable(test_tcp test_tcp.cpp ../TcpServer.cpp)
add_executable(test_log_server 
test_log_server.cpp  
../TcpServer.cpp 
../LogServerThread.cpp)

set_target_properties(test_basic           PROPERTIES COMPILE_FLAGS "-g")
set_target_properties(test_with_log_server PROPERTIES COMPILE_FLAGS "-g -pthread -DENABLE_LOG_SERVER")
set_target_properties(test_tcp             PROPERTIES COMPILE_FLAGS "-g -pthread")
set_target_properties(test_log_server      PROPERTIES COMPILE_FLAGS "-g -pthread")

target_link_libraries(test_basic TraceDebug stdc++)
target_link_libraries(test_with_log_server Util TraceDebugWithTcpServer stdc++)
target_link_libraries(test_tcp Util TraceDebug stdc++)
target_link_libraries(test_log_server Util TraceDebug)