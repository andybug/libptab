# --- dependency checks ---

include(CheckIncludeFileCXX)

check_include_file_cxx(cassert HAVE_CASSERT)
check_include_file_cxx(cstdio HAVE_CSTDIO)
check_include_file_cxx(cstdlib HAVE_CSTDLIB)
check_include_file_cxx(iostream HAVE_IOSTREAM)
check_include_file_cxx(sstream HAVE_SSTREAM)
check_include_file_cxx(stdexcept HAVE_STDEXCEPT)
check_include_file_cxx(string HAVE_STRING)
check_include_file_cxx(vector HAVE_VECTOR)
check_include_file_cxx(tclap/CmdLine.h HAV_TCLAP_H)


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
		ptab-library
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
