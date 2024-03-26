find_package(Catch2 REQUIRED)

define_property(GLOBAL PROPERTY CHECK_TARGET BRIEF_DOCS "." FULL_DOCS ".")
set_property(GLOBAL PROPERTY CHECK_TARGET "check")

if (NOT TARGET tests)
	add_custom_target(tests)
endif()

function(declare_test name)
	add_executable(${name} EXCLUDE_FROM_ALL ${ARGN})
	target_link_libraries(${name} PRIVATE ${PROJECT_NAME} Catch2::Catch2WithMain)
	add_custom_target(check_${name} COMMAND ${name})
	add_dependencies(tests ${name})

	get_property(check_target GLOBAL PROPERTY CHECK_TARGET)
	list(APPEND check_target COMMAND cmake -E cmake_echo_color --white --bold ${name} COMMAND ${name} DEPENDS ${name})
	set_property(GLOBAL PROPERTY CHECK_TARGET "${check_target}")

	add_test(NAME ${name} COMMAND ${name})
endfunction()

function(declare_tests prefix)
	foreach(test ${ARGN})
		declare_test(${prefix}${test} ${test}.cpp)
	endforeach()
endfunction()

declare_tests(dr_param_
	"std_optional"
	"yaml"
	"yaml_decompose"
	"yaml_preprocess"
)

get_property(check_target GLOBAL PROPERTY CHECK_TARGET)
add_custom_target(${check_target} USES_TERMINAL)

if (TARGET run_tests)
	add_dependencies(run_tests check)
endif()
