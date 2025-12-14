@echo off
echo Building SimpleRunner with full particle system...

REM 清理构建目录
if exist build rmdir /s /q build

REM 创建构建目录
mkdir build
cd build

REM 配置CMake
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

REM 编译
echo Compiling...
cmake --build . --config Release

REM 复制DLL文件
echo Copying DLLs...
if exist Release (
    copy "D:\Softwares\SFML-2.5.1\bin\sfml-*.dll" Release\
)

REM 运行游戏
cd Release
if exist SimpleRunner.exe (
    echo Running game...
    SimpleRunner.exe
) else (
    echo Error: Game executable not found!
)

pause