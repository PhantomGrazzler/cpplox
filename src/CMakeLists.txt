add_executable(cpplox)

target_sources(cpplox PRIVATE
    main.cpp
    chunk.cpp
    debug.cpp

    chunk.hpp
    debug.hpp
    value.hpp
 )

 target_link_libraries(cpplox
    PRIVATE project_warnings
)
