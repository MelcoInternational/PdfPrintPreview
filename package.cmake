IF(DEFINED CGET_CORE_DIR)
  include(${CGET_CORE_DIR}/.cget/core.cmake)
ELSE()
  include(.cget/core.cmake)
ENDIF()

FILE(WRITE "${CMAKE_BINARY_DIR}/set_freetype_library.cmake" "")

CGET_HAS_DEPENDENCY(FreeType NUGET_PACKAGE FreeType NUGET_VERSION 2.7.1.1 GIT git://git.sv.nongnu.org/freetype/freetype2.git VERSION VER-2-7-1 NO_FIND_VERSION OPTIONS -DBUILD_SHARED_LIBS:BOOL=true)

if(MSVC)
	CGET_HAS_DEPENDENCY(iconv NUGET_PACKAGE libiconv NUGET_VERSION 1.14.0.11 GIT git://git.savannah.gnu.org/libiconv.git VERSION v1.14 NO_FIND_PACKAGE ALLOW_SYSTEM)

	#Freetype has a bad config file; keyed to a very particular version of the library. This fixs that.
	if(CGET_FreeType_INSTALL_DIR)
		SET(FREETYPE_LIBRARY "${CGET_FreeType_INSTALL_DIR}/lib/freetype271.lib")
	else()
		SET(FREETYPE_LIBRARY "${CGET_INSTALL_DIR}/lib/freetype271.lib")
	endif()
	
	# We have to propagate the fix to the poppler make file
	STRING(REPLACE " " "\\ " FREETYPE_LIBRARY "${FREETYPE_LIBRARY}")
	FILE(APPEND "${CMAKE_BINARY_DIR}/set_freetype_library.cmake" "SET(FREETYPE_LIBRARY \"${FREETYPE_LIBRARY}\" CACHE STRING \"\" FORCE)\n")
	FILE(APPEND "${CMAKE_BINARY_DIR}/set_freetype_library.cmake" "SET(ICONV_CONST \"const\" CACHE STRING \"\" FORCE)")
else()
#	CGET_HAS_DEPENDENCY(Freetype GIT git://git.savannah.gnu.org/freetype/freetype2.git VERSION VER-2-7-1)
	FILE(APPEND "${CMAKE_BINARY_DIR}/set_freetype_library.cmake" "SET(FREETYPE_INCLUDE_DIR \"${CGET_INSTALL_DIR}/include/freetype2\" CACHE STRING \"\" FORCE)\n")

#        CGET_HAS_DEPENDENCY(libpng GIT git://git.code.sf.net/p/libpng/code VERSION v1.6.29 NO_FIND_PACKAGE)
	CGET_HAS_DEPENDENCY(Fontconfig GIT git://anongit.freedesktop.org/fontconfig VERSION 2.12.1 NO_FIND_PACKAGE PORTS_PACKAGE1 Fontconfig BREW_PACKAGE Fontconfig)
#	CGET_HAS_DEPENDENCY(Fontconfig GITHUB CMakePorts/fontconfig COMMIT_ID 7f4996621 NO_FIND_PACKAGE PORTS_PACKAGE1 Fontconfig BREW_PACKAGE1 Fontconfig)
endif()

# Get find configs for poppler
CGET_HAS_DEPENDENCY(extra-cmake-modules GITHUB KDE/extra-cmake-modules NO_FIND_PACKAGE COMMIT_ID fd60f2b893d0b07f96f0fd715109cbd8d4e66140)
IF(CGET_extra-cmake-modules_INSTALL_DIR)
	LIST(APPEND CMAKE_MODULE_PATH "${CGET_extra-cmake-modules_INSTALL_DIR}/share/ECM/find-modules")
ELSE()
	LIST(APPEND CMAKE_MODULE_PATH "${CGET_INSTALL_DIR}/share/ECM/find-modules")
ENDIF()

CGET_HAS_DEPENDENCY(Poppler GITHUB MelcoInternational/Poppler
		OPTIONS_FILE "${CMAKE_BINARY_DIR}/set_freetype_library.cmake"
	OPTIONS
	-DWITH_Cairo:BOOL=OFF
	-DWITH_PNG:BOOL=OFF
	-DENABLE_LIBPNG:BOOL=OFF
	-DWITH_FONTCONFIGURATION_FONTCONFIG=OFF
	-DENABLE_LIBOPENJPEG=""
	-DBUILD_GTK_TESTS:BOOL=OFF
	-DBUILD_QT4_TESTS:BOOL=OFF
	-DBUILD_QT5_TESTS:BOOL=OFF
	-DBUILD_CPP_TESTS:BOOL=OFF
	-DENABLE_UTILS:BOOL=OFF
	-DENABLE_SPLASH:BOOL=ON
	COMMIT_ID 07cf478a58211da333c5b12cbd3fea64b4edd314
	)
 
