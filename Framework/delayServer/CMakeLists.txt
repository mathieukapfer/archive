# Create libs
add_library(DelayServer  STATIC 	DelayServer TimerInfoList TimerDb.cpp TimerEventI)

target_link_libraries(DelayServer TraceDebug Util rt pthread)