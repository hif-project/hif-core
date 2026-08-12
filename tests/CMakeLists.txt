# -----------------------------------------------------------------------------
# @brief  : Tests cmake file.
# @author : Enrico Fraccaroli
# -----------------------------------------------------------------------------

add_executable(code_info_source_line_test ${CMAKE_CURRENT_SOURCE_DIR}/code_info_source_line_test.cpp)
target_include_directories(code_info_source_line_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(code_info_source_line_test PRIVATE hif::hif)
target_compile_features(code_info_source_line_test PRIVATE cxx_std_17)

add_test(
    NAME code_info_source_line
    COMMAND code_info_source_line_test ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/and2_known_good.hif.xml
)
