cmake_minimum_required(VERSION 3.27)

set(Project "Resources")

# SET THIS VARIABLE TO ENABLE UTILITY TOOL
# WITH OUT RESOURCES_UTILITY, THE TOOL 
# FUNCTIONS ARE DISABLED.
set(RESOURCES_UTILITY true)

set(Header Resources.hpp)

set(Source Resources.cpp)

if (NOT DEFINED RESOURCES_UTILITY)
    add_library(${Project} ${Source} ${Header})
else()
    set(RESOURCES_UTILITY true)
    add_compile_definitions(RESOURCES_UTILITY)
    add_library(${Project} ${Source} ${Header})
    add_executable(${Project}_bin main.cpp)
    target_link_libraries(${Project}_bin PRIVATE ${Project})
    set_property(TARGET ${Project}_bin PROPERTY OUTPUT_NAME ${Project})
endif()

project(${Project} LANGUAGES CXX)

set_target_properties(${Project} PROPERTIES CXX_STANDARD 23)

target_include_directories(${Project} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${Project} PROPERTIES PUBLIC_HEADER ${Header})

find_package(unofficial-sqlite3 CONFIG REQUIRED)
target_link_libraries(${Project} PUBLIC unofficial::sqlite3::sqlite3)

find_package(Boost REQUIRED COMPONENTS iostreams)
target_link_libraries(${Project} PUBLIC Boost::boost Boost::iostreams)

find_package(ZLIB REQUIRED)
target_link_libraries(${Project} PUBLIC ZLIB::ZLIB)

include(GNUInstallDirs)
install(TARGETS ${Project} ${Project}_bin
    EXPORT ${Project}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)