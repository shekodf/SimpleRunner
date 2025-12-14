# cmake_minimum_required(VERSION 3.16)
# project(SimpleRunner LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # 设置SFML路径（使用SFML 2.5.1）
# set(SFML_DIR "D:/Softwares/SFML-2.5.1/lib/cmake/SFML")
# find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

# # 收集所有源文件
# # file(GLOB_RECURSE SOURCES src/*.cpp)
# file(GLOB_RECURSE SOURCES 
#     src/*.cpp
#     src/entities/*.cpp
#     src/core/*.cpp
#     src/systems/*.cpp
#     src/utils/*.cpp
# )

# # 创建可执行文件
# add_executable(SimpleRunner ${SOURCES})

# # 包含目录
# target_include_directories(SimpleRunner PRIVATE src)

# # 链接SFML库（SFML 2.5使用不同方式）
# target_link_libraries(SimpleRunner
#     sfml-graphics
#     sfml-window
#     sfml-system
#     sfml-audio
# )

# # Windows特定设置
# if(WIN32)
#     target_link_libraries(SimpleRunner
#         opengl32
#         winmm
#         gdi32
#     )
    
#     # 复制DLL文件（SFML 2.5的DLL有版本号）
#     add_custom_command(TARGET SimpleRunner POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "D:/Softwares/SFML-2.5.1/bin/sfml-graphics-2.dll"
#         $<TARGET_FILE_DIR:SimpleRunner>
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "D:/Softwares/SFML-2.5.1/bin/sfml-window-2.dll"
#         $<TARGET_FILE_DIR:SimpleRunner>
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "D:/Softwares/SFML-2.5.1/bin/sfml-system-2.dll"
#         $<TARGET_FILE_DIR:SimpleRunner>
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "D:/Softwares/SFML-2.5.1/bin/sfml-audio-2.dll"
#         $<TARGET_FILE_DIR:SimpleRunner>
#     )
# endif()


cmake_minimum_required(VERSION 3.16)
project(SimpleRunner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置SFML路径
set(SFML_DIR "D:/Softwares/SFML-2.5.1/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

# 收集所有源文件
file(GLOB_RECURSE SOURCES 
    src/*.cpp
    src/core/*.cpp
    src/entities/*.cpp
    src/systems/*.cpp
    src/utils/*.cpp
)

# 创建可执行文件
add_executable(SimpleRunner ${SOURCES})

# 包含目录
target_include_directories(SimpleRunner PRIVATE src)

# 链接SFML库
target_link_libraries(SimpleRunner
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Windows特定设置
if(WIN32)
    target_link_libraries(SimpleRunner
        opengl32
        winmm
        gdi32
    )
    
    # 复制DLL文件
    add_custom_command(TARGET SimpleRunner POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "D:/Softwares/SFML-2.5.1/bin/sfml-graphics-2.dll"
        $<TARGET_FILE_DIR:SimpleRunner>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "D:/Softwares/SFML-2.5.1/bin/sfml-window-2.dll"
        $<TARGET_FILE_DIR:SimpleRunner>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "D:/Softwares/SFML-2.5.1/bin/sfml-system-2.dll"
        $<TARGET_FILE_DIR:SimpleRunner>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "D:/Softwares/SFML-2.5.1/bin/sfml-audio-2.dll"
        $<TARGET_FILE_DIR:SimpleRunner>
    )
endif()