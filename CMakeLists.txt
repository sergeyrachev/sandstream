cmake_minimum_required(VERSION 3.6)
if(NOT DEFINED project_version)
    set(project_version 0.0.0)
endif()
if(NOT DEFINED project_revision)
    set(project_revision 0000000)
endif()
set(project_version_revision ${project_version}+g${project_revision})
string(TIMESTAMP project_birthday UTC)

message("Build ${project_version_revision} at ${project_birthday}")

project(sandstream VERSION ${project_version})

configure_file(depends/sdk/src/version.cpp.in version.cpp)
configure_file(depends/sdk/src/birthday.cpp.in birthday.cpp)

add_executable(sandstream
    ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/birthday.cpp
    src/main.cpp
    src/demuxer.cpp
    src/detector.cpp
    src/pes_packet.cpp
    src/ts_packet.cpp)
target_compile_features(sandstream PRIVATE cxx_std_11 )
set_target_properties(sandstream PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
target_include_directories(sandstream
    PRIVATE
    src/
    depends/sdk/src )

install(TARGETS sandstream DESTINATION bin/ )
install(PROGRAMS README.md LICENSE DESTINATION share/)

set(CPACK_PACKAGE_VERSION ${project_version_revision})
set(CPACK_PACKAGE_CONTACT "contact@me.directly")
set(CPACK_RPM_PACKAGE_REQUIRES "")
set(CPACK_RPM_PACKAGE_AUTOREQPROV 0)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)

include(CPack)

