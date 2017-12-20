# Create libs
add_library(Util  STATIC 
                filter 
                Sem
                SemPosix
                SemCollection
                thread
                Chrono)

target_link_libraries(Util TraceDebug pthread)
