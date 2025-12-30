add_executable(cpplox)

target_sources(cpplox PRIVATE
    main.cpp
    debug.cpp

    chunk.hpp
    debug.hpp
 )

 target_link_libraries(cpplox
    PRIVATE project_warnings
)
