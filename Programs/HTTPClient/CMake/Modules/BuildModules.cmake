macro (BuildModules)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	                    ${CMAKE_CURRENT_SOURCE_DIR}/../../FoundationKit
	                    ${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParty/curl/include/win32/curl)
    file(GLOB_RECURSE SOURCE_LIST "${CMAKE_CURRENT_SOURCE_DIR}/HTTPClient/*.cpp")
	file(GLOB_RECURSE HEADER_LIST "${CMAKE_CURRENT_SOURCE_DIR}/HTTPClient/*.hpp")

	add_library(HTTPClient STATIC ${SOURCE_LIST} ${HEADER_LIST})
	set_target_properties(HTTPClient
	    PROPERTIES
	    PUBLIC_HEADER "${HEADER_LIST}")          
endmacro(BuildModules)


