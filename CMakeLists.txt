cmake_minimum_required(VERSION 3.5)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(PdfPrintPreview)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Toolchain/)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

include(package.cmake)
find_package(Poppler REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})

add_executable(PdfPrintPreview main.cpp)
target_link_libraries(PdfPrintPreview
  Qt5::Widgets
  Qt5::Core
  Qt5::PrintSupport
  ${Poppler_LIBRARIES}
  )

set_target_properties(PdfPrintPreview PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
install(TARGETS PdfPrintPreview DESTINATION bin)
