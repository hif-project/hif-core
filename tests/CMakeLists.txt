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

# Regression (hif-core#20): HifFactory::library's file-name parameter is a
# `const std::string &`, and nine call sites passed a literal `nullptr` for it.
# Constructing a std::string from a null char* threw std::logic_error out of
# the package constructor, so std.textio, ieee.std_logic_textio and
# ieee.std_logic_misc could not be built at all and the process terminated.
# See vhdl_standard_package_construction_test.cpp.
add_executable(vhdl_standard_package_construction_test
    ${CMAKE_CURRENT_SOURCE_DIR}/vhdl_standard_package_construction_test.cpp)
target_include_directories(vhdl_standard_package_construction_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(vhdl_standard_package_construction_test PRIVATE hif::hif)
target_compile_features(vhdl_standard_package_construction_test PRIVATE cxx_std_17)

add_test(NAME vhdl_standard_package_construction COMMAND vhdl_standard_package_construction_test)

# Regression (hif-frontend#32): SimplifyMap never folded a reduction over a
# constant - map(BitvectorValue *) carried a `// TODO` for op_andrd/op_orrd/
# op_xorrd and map(IntValue *) had no reduction branch at all. Verilog lowers
# `if (<cond>)` to `or_reduce(<cond>)`, so an `if generate` with an else branch
# could not resolve its condition and aborted whenever generate expansion ran.
# See constant_reduction_folding_test.cpp.
add_executable(constant_reduction_folding_test ${CMAKE_CURRENT_SOURCE_DIR}/constant_reduction_folding_test.cpp)
target_include_directories(constant_reduction_folding_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(constant_reduction_folding_test PRIVATE hif::hif)
target_compile_features(constant_reduction_folding_test PRIVATE cxx_std_17)

add_test(NAME constant_reduction_folding COMMAND constant_reduction_folding_test)

# Regression (hif-core#24): expanding a `for generate` substituted the
# iteration ordinal for the genvar, so a loop over 0, 2, 4 was elaborated as
# 0, 1, 2 - a different design, with exit status 0. The same arithmetic
# truncated the iteration count for a span that is not a multiple of the step,
# and started an `op_gt` range one below the lowest value the index reaches.
# See for_generate_step_expansion_test.cpp.
add_executable(for_generate_step_expansion_test
    ${CMAKE_CURRENT_SOURCE_DIR}/for_generate_step_expansion_test.cpp)
target_include_directories(for_generate_step_expansion_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(for_generate_step_expansion_test PRIVATE hif::hif)
target_compile_features(for_generate_step_expansion_test PRIVATE cxx_std_17)

add_test(NAME for_generate_step_expansion COMMAND for_generate_step_expansion_test)

# Regression (hif-core#22): TerminalPrefixOptions had no default member
# initializers, so the defaults its header documents did not exist - the
# default argument value-initialized every flag to false, the opposite of the
# documented value for three of them - and a default-initialized local left
# every member a call site did not assign indeterminate. The first half is what
# made hif2vhdl abort on any bit-select or part-select assignment target
# (hif-backend#103). See terminal_prefix_options_defaults_test.cpp.
add_executable(terminal_prefix_options_defaults_test
    ${CMAKE_CURRENT_SOURCE_DIR}/terminal_prefix_options_defaults_test.cpp)
target_include_directories(terminal_prefix_options_defaults_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(terminal_prefix_options_defaults_test PRIVATE hif::hif)
target_compile_features(terminal_prefix_options_defaults_test PRIVATE cxx_std_17)

add_test(NAME terminal_prefix_options_defaults COMMAND terminal_prefix_options_defaults_test)

# Regression (hif-backend#94): a Verilog shift by a vector amount regenerated as
# invalid VHDL - to_integer(to_unsigned(a)) sll to_integer(to_unsigned(n)) -
# because the vector-to-integer cast called to_unsigned, the shifted operand was
# given the count's integer type, and VHDLAnalysis accepted a shift by a vector.
# See vhdl_shift_and_to_integer_test.cpp.
add_executable(vhdl_shift_and_to_integer_test
    ${CMAKE_CURRENT_SOURCE_DIR}/vhdl_shift_and_to_integer_test.cpp)
target_include_directories(vhdl_shift_and_to_integer_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(vhdl_shift_and_to_integer_test PRIVATE hif::hif)
target_compile_features(vhdl_shift_and_to_integer_test PRIVATE cxx_std_17)

add_test(NAME vhdl_shift_and_to_integer COMMAND vhdl_shift_and_to_integer_test)
