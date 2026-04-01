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

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG        master # You can also use a specific tag like "0.8.0" for stability
)

FetchContent_MakeAvailable(cmark yaml-cpp)

find_package(wxWidgets COMPONENTS core stc base html REQUIRED)
include(${wxWidgets_USE_FILE})

add_library(markdownWindow STATIC ./src/MarkdownWindow.cpp ./src/MarkdownWindowFileEvents.cpp
  ./src/MarkdownWindowEditEvents.cpp ./src/MarkdownWindowViewEvents.cpp ./src/MarkdownWindowHelpEvents.cpp
  ./src/MarkdownWindowUtility.cpp)

add_executable(${PROJECT_NAME} ./main.cpp)
target_link_libraries(markdownWindow PUBLIC ${wxWidgets_LIBRARIES} cmark yaml-cpp)
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} cmark markdownWindow)
message(STATUS "==== WXWIDGETS LIBRARIES FOUND: ${wxWidgets_LIBRARIES} ====")

