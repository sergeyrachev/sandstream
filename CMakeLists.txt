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



configure_file(src/version.cpp.in version.cpp)
configure_file(src/birthday.cpp.in birthday.cpp)

add_executable(sandstream
    ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/birthday.cpp
    src/main.cpp src/main.h
    src/demuxer.cpp src/demuxer.h
    src/callback_es.cpp src/callback_es.h
    src/callback_es_factory.cpp src/callback_es_factory.h
    src/callback_ts_packet.cpp src/callback_ts_packet.h
    src/storage.cpp src/storage.h
    src/section_header.cpp src/section_header.h
    src/masked_two_bytes_value.cpp src/masked_two_bytes_value.h
    src/parser_psi.cpp src/parser_psi.h
    src/callback_pat.cpp src/callback_pat.h
    src/callback_pmt.cpp src/callback_pmt.h
    src/parser_pat.cpp src/parser_pat.h
    src/parser_pmt.cpp src/parser_pmt.h
    src/parser_pes.cpp src/parser_pes.h
    src/packetizer.cpp src/packetizer.h
    src/pat.cpp src/pat.h
    src/pmt.cpp src/pmt.h
    src/ts_packet.cpp src/ts_packet.h
    src/parser_pat_details.cpp src/parser_pat_details.h src/parser_pmt_details.cpp src/parser_pmt_details.h)
target_compile_features(sandstream PRIVATE cxx_std_11 )
set_target_properties(sandstream PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
target_include_directories(sandstream
    PRIVATE
    src/
    depends/sdk/src )

install(TARGETS sandstream DESTINATION bin/ )
install(PROGRAMS README.md LICENSE DESTINATION share/)

set(gtest_force_shared_crt ON)
add_subdirectory(depends/googletest depends/googletest EXCLUDE_FROM_ALL)
set(GTEST_LIBRARIES gmock_main gtest)
get_target_property(GTEST_INCLUDE_DIRECTORIES gtest INTERFACE_INCLUDE_DIRECTORIES)

add_executable(sandstream_gtest
    src/masked_two_bytes_value.cpp test/ut_masked_two_bytes_value.cpp
    src/packetizer.cpp test/ut_packetizer.cpp
    src/ts_packet.cpp test/ut_ts_packet.cpp
    src/storage.cpp test/ut_storage.cpp
    src/parser_pat.cpp test/ut_parser_pat.cpp
    src/parser_psi.cpp test/ut_parser_psi.cpp
    src/parser_pmt.cpp test/ut_parser_pmt.cpp
    src/parser_pes.cpp test/ut_parser_pes.cpp
    test/mock_callback_pat.cpp test/mock_callback_pat.h
    test/mock_callback_pmt.cpp test/mock_callback_pmt.h
    test/mock.cpp test/mock.h
    test/mock_parser_psi.cpp test/mock_parser_psi.h
    test/mock_callback_es.cpp test/mock_callback_es.h)
target_link_libraries(sandstream_gtest PRIVATE ${GTEST_LIBRARIES})
target_include_directories(sandstream_gtest PRIVATE ${GTEST_INCLUDE_DIRECTORIES} src test)

enable_testing()
add_test( unittests sandstream_gtest )

set(CPACK_PACKAGE_VERSION ${project_version_revision})
set(CPACK_PACKAGE_CONTACT "contact@me.directly")
set(CPACK_RPM_PACKAGE_REQUIRES "")
set(CPACK_RPM_PACKAGE_AUTOREQPROV 0)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)

include(CPack)

