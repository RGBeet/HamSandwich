file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
HamSandwich_add_executable(
	sylabulus
	ICON sylabulus.ico
	SOURCES ${sources}
)
target_sources(sylabulus PRIVATE sylabulus.manifest)
target_link_libraries(sylabulus steam_api minizip strnatcmp)

if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$" AND NOT MSVC)
	target_compile_options(sylabulus PRIVATE
		# Lots of warnings, but some exclusions
		-Wall -Wextra -Wpedantic
		-Wno-unused-parameter
		-Wno-unused-but-set-variable
		-Wno-unknown-warning-option
	)
endif()
