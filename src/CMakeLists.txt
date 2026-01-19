add_executable(cpplox)

target_sources(cpplox PRIVATE
    main.cpp
    chunk.cpp
    compiler.cpp
    debug.cpp
    scanner.cpp
    vm.cpp

    chunk.hpp
    compiler.hpp
    debug.hpp
    scanner.hpp
    value.hpp
    vm.hpp
 )

 target_link_libraries(cpplox
    PRIVATE project_warnings
)

set_target_properties(cpplox PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)"
)
