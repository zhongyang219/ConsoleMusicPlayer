# CMakeList.txt: MusicPlayer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
# 使用Unicode
add_definitions(-DUNICODE -D_UNICODE)

MESSAGE(STATUS "current build type is ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "current architecture is ${CMAKE_ARCHITECTURE}")
MESSAGE(STATUS "proj source dir: ${PROJECT_SOURCE_DIR}")

if (CMAKE_ARCHITECTURE MATCHES "x64")
	link_libraries("${PROJECT_SOURCE_DIR}/MusicPlayer/bass_x64.lib")
elseif (CMAKE_ARCHITECTURE MATCHES "x86")
	link_libraries("${PROJECT_SOURCE_DIR}/MusicPlayer/bass.lib")
endif()

# 将源代码添加到此项目的可执行文件。
add_executable (MusicPlayer "Main.cpp" "Common.h" "Console.h" "Lyrics.h" "Player.h" "bass.h" "resource.h" "WinVersionHelper.cpp" "WinVersionHelper.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MusicPlayer PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。

