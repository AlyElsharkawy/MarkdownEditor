cmake_minimum_required(VERSION 3.11)
project(Markdown-Editor)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)

include_directories("./include")

# Setting output lib and bin directories for debug and release mode 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../lib/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../lib/$<CONFIG>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/$<CONFIG>")
#set(BUILD_SHARED_LIBS OFF CACHE BOOL "Force static libraries" FORCE)
FetchContent_Declare(
    cmark
    GIT_REPOSITORY https://github.com/commonmark/cmark.git
    GIT_TAG 0.31.1
)

FetchContent_MakeAvailable(cmark)

find_package(wxWidgets COMPONENTS core stc base html webview REQUIRED)
include(${wxWidgets_USE_FILE})

add_library(markdownWindow STATIC ./src/MarkdownWindow.cpp ./src/MarkdownWindowFileEvents.cpp
  ./src/MarkdownWindowEditEvents.cpp ./src/MarkdownWindowViewEvents.cpp ./src/MarkdownWindowHelpEvents.cpp
  ./src/MarkdownWindowUtility.cpp)
add_library(SimpleHtmlBackend STATIC ./src/SimpleHtmlBackend.cpp)
add_library(WebViewHtmlBackend STATIC ./src/WebViewHtmlBackend.cpp)
add_library(HtmlBackendFactory STATIC ./src/HtmlBackendFactory.cpp)
target_link_libraries(SimpleHtmlBackend ${wxWidgets_LIBRARIES})
target_link_libraries(WebViewHtmlBackend ${wxWidgets_LIBRARIES})
target_link_libraries(HtmlBackendFactory SimpleHtmlBackend WebViewHtmlBackend)

add_executable(${PROJECT_NAME} ./main.cpp)
if(CMAKE_BUILD_TYPE STREQUAL "debug")
  target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_MODE)
endif()

target_link_libraries(markdownWindow PUBLIC ${wxWidgets_LIBRARIES} cmark)
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} cmark markdownWindow SimpleHtmlBackend
  HtmlBackendFactory)
message(STATUS "==== WXWIDGETS LIBRARIES FOUND: ${wxWidgets_LIBRARIES} ====")

