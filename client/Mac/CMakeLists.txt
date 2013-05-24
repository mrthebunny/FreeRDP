cmake_minimum_required (VERSION 2.6)
project(MacFreeRDP-library)

# add directory for App
add_subdirectory(cli)

set(MODULE_NAME "MacFreeRDP-library")
set(MODULE_PREFIX "FREERDP_CLIENT_MAC-LIB")

# set(FRAMEWORK_HEADERS_PATH /System/Library/Frameworks/Cocoa.framework/Versions/A/Headers/)
# include_directories(${FRAMEWORK_HEADERS_PATH} /System/Library/Frameworks) 
	
# set(CMAKE_OSX_SYSROOT MacOSX10.7.sdk)
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -mmacosx-version-min=10.4")

# Import libraries
find_library(FOUNDATION_LIBRARY Foundation)
find_library(COCOA_LIBRARY Cocoa)
find_library(APPKIT_LIBRARY AppKit)

set(MACOSX_BUNDLE_INFO_STRING "MacFreeRDP-library")
set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.freerdp.mac")
set(MACOSX_BUNDLE_BUNDLE_IDENTIFIER "FreeRDP-library.Mac")
set(MACOSX_BUNDLE_LONG_VERSION_STRING "MacFreeRDP library Version 1.1")
set(MACOSX_BUNDLE_BUNDLE_NAME "MacFreeRDP-library")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING 1.1.0)
set(MACOSX_BUNDLE_BUNDLE_VERSION 1.1.0)
set(MACOSX_BUNDLE_COPYRIGHT "Copyright 2013. All Rights Reserved.")

mark_as_advanced(COCOA_LIBRARY FOUNDATION_LIBRARY APPKIT_LIBRARY)
set(EXTRA_LIBS ${COCOA_LIBRARY} ${FOUNDATION_LIBRARY} ${APPKIT_LIBRARY})

add_library(${MODULE_NAME}
	SHARED
	MRDPView.h
	MRDPView.m
	MRDPCursor.m
	PasswordDialog.m
	en.lproj/InfoPlist.strings)

# configures the framework to always be looked for in the application bundle in the Frameworks sub-folder.
SET_TARGET_PROPERTIES( ${MODULE_NAME}  PROPERTIES XCODE_ATTRIBUTE_INSTALL_PATH @executable_path/../Frameworks/  )
          
message(STATUS "executable output path: " ${EXECUTABLE_OUTPUT_PATH}) 
message(STATUS "library output path: " ${LIBRARY_OUTPUT_PATH}) 
 
#	SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/build123")
#	SET(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/build123")
   
 set_target_properties( ${MODULE_NAME}  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${EXECUTABLE_OUTPUT_PATH} 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EXECUTABLE_OUTPUT_PATH}
)           
           
# Automatic ref counting
# temporary turn off for x86_64 build issue
# set_target_properties(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)

# Support for automatic reference counting requires non-fragile abi.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fobjc-nonfragile-abi")

# XCode project architecture to native architecture of build machine
# -----------------------------------------------------------------------------------------------------
# Issue: Had some issues with FreeRDP project building only 64 bit and
# MacFreeRDP attempting to link to both 32 and 64 for dual target.
# In the future the FreeRDP Xcode project should be pulled in for a couple of reasons:
# 1) better step-into debugging 2) automatic dependency compilation and multi-arch compilation + linkage
# If you know the solutions for 1 and 2, please add below.
set_target_properties(${MODULE_NAME} PROPERTIES XCODE_ATTRIBUTE_ARCHS "$(NATIVE_ARCH_ACTUAL)")

# Set the info plist to the custom instance
#	MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist

set(MODULE_VERSION, 1.1.0)

set_target_properties(${MODULE_NAME} PROPERTIES 
	FRAMEWORK TRUE
	MACOSX_FRAMEWORK_IDENTIFIER com.awakecoding.${MODULE_NAME}
	FRAMEWORK_VERSION 1.1.0
	MACOSX_FRAMEWORK_SHORT_VERSION_STRING 1.1.0
	MACOSX_FRAMEWORK_BUNDLE_VERSION 1.1.0
	PUBLIC_HEADER "MRDPView.h"
    INSTALL_NAME_DIR "@executable_path/../../Frameworks"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
    BUILD_WITH_INSTALL_RPATH 1
)         

set(${MODULE_PREFIX}_LIBS ${${MODULE_PREFIX}_LIBS} ${EXTRA_LIBS} freerdp-client)

# Set a list of the dependent targets used by the application. There should be a way to get this list automatically
# from cmake, but for now I put it down manually. I got the references by calling
# otool -L MacFreeRDP-client

set(DEPENDENCIES  
	winpr-asn1
	winpr-heap		
	winpr-sspi
	winpr-bcrypt		
	winpr-input		
	winpr-sspicli
	winpr-credentials	
	winpr-interlocked	
	winpr-synch
	winpr-credui		
	winpr-io		
	winpr-sysinfo
	winpr-crt		
	winpr-library		
	winpr-thread
	winpr-crypto		
	winpr-makecert-tool	
	winpr-timezone
	winpr-dsparse		
	winpr-path		
	winpr-utils
	winpr-environment	
	winpr-pipe		
	winpr-winhttp
	winpr-error		
	winpr-pool		
	winpr-winsock
	winpr-file		
	winpr-registry
	winpr-handle		
	winpr-rpc
	freerdp-utils
	freerdp-gdi
	freerdp-rail
	freerdp-cache
	freerdp-codec
	freerdp-primitives
	freerdp-core
	freerdp-crypto
	freerdp-client
	freerdp-locale
	freerdp-common
	)


## Modify the install name path of the dependent libraries so that they can be located by the bundling app.
# foreach(LIB ${DEPENDENCIES})
# set_target_properties(${LIB} PROPERTIES 
#     INSTALL_NAME_DIR "@executable_path/../Frameworks/${MODULE_NAME}.framework/Contents"
#     BUILD_WITH_INSTALL_RPATH 1
# )         
# endforeach()

set_complex_link_libraries(VARIABLE ${MODULE_PREFIX}_LIBS MONOLITHIC ${MONOLITHIC_BUILD}
	MODULE freerdp
	MODULES freerdp-core freerdp-cache freerdp-gdi freerdp-codec freerdp-primitives freerdp-rail freerdp-utils)

set_complex_link_libraries(VARIABLE ${MODULE_PREFIX}_LIBS MONOLITHIC ${MONOLITHIC_BUILD}
	MODULE winpr
	MODULES winpr-input winpr-crt winpr-utils)

set_target_properties(${MODULE_NAME} PROPERTIES LINK_INTERFACE_LIBRARIES "")
target_link_libraries(${MODULE_NAME} ${${MODULE_PREFIX}_LIBS})

set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Client/Mac")

# Add a post-build event to copy the dependent libraries in the framework bundle 
# Call install_name_tool to reassign the library install name
foreach(LIB ${DEPENDENCIES})
	message("adding post-build dependency: ${LIB}")
	add_custom_command(TARGET ${MODULE_NAME} POST_BUILD 
  		COMMAND "${CMAKE_COMMAND}" -E copy 
     			"$<TARGET_FILE:${LIB}>"
     			"${CMAKE_CURRENT_BINARY_DIR}/$(CONFIGURATION)/${MODULE_NAME}.framework/Contents/$<TARGET_FILE_NAME:${LIB}>"
  		COMMENT "Copying ${LIB} to output directory"
  		COMMAND install_name_tool -change "$<TARGET_FILE:${LIB}>" "@executable_path/../Frameworks/${MODULE_NAME}.framework/Contents/$<TARGET_FILE_NAME:${LIB}>" "${CMAKE_CURRENT_BINARY_DIR}/$(CONFIGURATION)/${MODULE_NAME}.framework/${MODULE_NAME}"
  		COMMENT Setting install name for ${LIB}
  		)
endforeach()


# Call install_name_tool to reassign the library install names in dependent libraries
foreach(DEST ${DEPENDENCIES})
foreach(LIB ${DEPENDENCIES})
	message("adding post-build dependency: ${LIB}")
	add_custom_command(TARGET ${MODULE_NAME} POST_BUILD 
  		COMMAND install_name_tool -change "$<TARGET_FILE:${LIB}>" "@executable_path/../Frameworks/${MODULE_NAME}.framework/Contents/$<TARGET_FILE_NAME:${LIB}>" "${CMAKE_CURRENT_BINARY_DIR}/$(CONFIGURATION)/${MODULE_NAME}.framework/Contents/$<TARGET_FILE_NAME:${DEST}>"
  		COMMENT Setting install name for ${LIB} in module ${DEST}
  		)
endforeach()
endforeach()