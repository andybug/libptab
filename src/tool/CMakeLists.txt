project(libptab)

# --- build tool ---

add_executable(
	ptab-tool
	column.cpp
	ptab.cpp
	tool.cpp
)

if("$ENV{PTAB_ENV_GCOV}" STREQUAL 1)
	target_link_libraries(
		ptab-tool
		ptab-static
		gcov
	)
else()
	target_link_libraries(
		ptab-tool
		ptab-library
	)
endif()

set_target_properties(
	ptab-tool PROPERTIES
	OUTPUT_NAME ptab
)

install(
	TARGETS ptab-tool
	RUNTIME DESTINATION bin
)
