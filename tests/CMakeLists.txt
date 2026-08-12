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

# Regression: VerilogAnalysis has no map(Int*, Int*) overload, so any binary
# expression combining two Int-typed operands under Verilog semantics
# silently fails to type. See verilog_analysis_int_int_test.cpp.
add_executable(verilog_analysis_int_int_test ${CMAKE_CURRENT_SOURCE_DIR}/verilog_analysis_int_int_test.cpp)
target_include_directories(verilog_analysis_int_int_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(verilog_analysis_int_int_test PRIVATE hif::hif)
target_compile_features(verilog_analysis_int_int_test PRIVATE cxx_std_17)

add_test(NAME verilog_analysis_int_int COMMAND verilog_analysis_int_int_test)
