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
set(MACOSX_BUNDLE_LONG_VERSION_STRING "MacFreeRDP library Version 0.1")
set(MACOSX_BUNDLE_BUNDLE_NAME "MacFreeRDP-library")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING 0.1.0)
set(MACOSX_BUNDLE_BUNDLE_VERSION 0.1.0)
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
SET_TARGET_PROPERTIES( ${MODULE_NAME}  PROPERTIES
           XCODE_ATTRIBUTE_INSTALL_PATH @executable_path/../Frameworks/  )
          
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

set(MODULE_VERSION, 0.1.0)

set_target_properties(${MODULE_NAME} PROPERTIES 
	FRAMEWORK TRUE
	MACOSX_FRAMEWORK_IDENTIFIER com.awakecoding.${MODULE_NAME}
	FRAMEWORK_VERSION 0.1.0
	MACOSX_FRAMEWORK_SHORT_VERSION_STRING 0.1.0
	MACOSX_FRAMEWORK_BUNDLE_VERSION 0.1.0
	PUBLIC_HEADER "MRDPView.h"
    INSTALL_NAME_DIR "@executable_path/../../Frameworks"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
    BUILD_WITH_INSTALL_RPATH 1
)         

set(${MODULE_PREFIX}_LIBS ${${MODULE_PREFIX}_LIBS} ${EXTRA_LIBS})

set(${MODULE_PREFIX}_LIBS ${${MODULE_PREFIX}_LIBS} freerdp-client)

set_complex_link_libraries(VARIABLE ${MODULE_PREFIX}_LIBS MONOLITHIC ${MONOLITHIC_BUILD}
	MODULE freerdp
	MODULES freerdp-core freerdp-cache freerdp-gdi freerdp-codec freerdp-primitives freerdp-rail freerdp-utils)

set_complex_link_libraries(VARIABLE ${MODULE_PREFIX}_LIBS MONOLITHIC ${MONOLITHIC_BUILD}
	MODULE winpr
	MODULES winpr-input winpr-crt winpr-utils)

target_link_libraries(${MODULE_NAME} ${${MODULE_PREFIX}_LIBS})

set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Client/Mac")
