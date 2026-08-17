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

# Regression: $clog2 was never registered in Verilog's standard library
# (only a `//$clog2` comment marker), unlike $time/$stime/$random. See
# verilog_clog2_registration_test.cpp.
add_executable(verilog_clog2_registration_test ${CMAKE_CURRENT_SOURCE_DIR}/verilog_clog2_registration_test.cpp)
target_include_directories(verilog_clog2_registration_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(verilog_clog2_registration_test PRIVATE hif::hif)
target_compile_features(verilog_clog2_registration_test PRIVATE cxx_std_17)

add_test(NAME verilog_clog2_registration COMMAND verilog_clog2_registration_test)

# Regression: _getSimplifiedSymbol_iteratedConcat returned a mixed-domain
# subtree - destination-form outer call over a source-form nested standard
# call - so `{$clog2(DEPTH){1'b1}}` could not resolve. See
# hif_simplified_symbol_iterated_concat_test.cpp.
add_executable(hif_simplified_symbol_iterated_concat_test
    ${CMAKE_CURRENT_SOURCE_DIR}/hif_simplified_symbol_iterated_concat_test.cpp)
target_include_directories(hif_simplified_symbol_iterated_concat_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(hif_simplified_symbol_iterated_concat_test PRIVATE hif::hif)
target_compile_features(hif_simplified_symbol_iterated_concat_test PRIVATE cxx_std_17)

add_test(NAME hif_simplified_symbol_iterated_concat COMMAND hif_simplified_symbol_iterated_concat_test)

# Regression: _getSimplifiedSymbol_withVerilogIntegers renamed the call but left
# its referenced Library in source form, which only stayed invisible while every
# input came straight out of the tree. See
# hif_simplified_symbol_verilog_integers_test.cpp.
add_executable(hif_simplified_symbol_verilog_integers_test
    ${CMAKE_CURRENT_SOURCE_DIR}/hif_simplified_symbol_verilog_integers_test.cpp)
target_include_directories(hif_simplified_symbol_verilog_integers_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(hif_simplified_symbol_verilog_integers_test PRIVATE hif::hif)
target_compile_features(hif_simplified_symbol_verilog_integers_test PRIVATE cxx_std_17)

add_test(NAME hif_simplified_symbol_verilog_integers COMMAND hif_simplified_symbol_verilog_integers_test)
