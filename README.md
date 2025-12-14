# Simple Runner - 2D SFML 游戏项目

一个使用 C++17 和 SFML 3.0.2 开发的 2D 小游戏，玩家控制角色躲避下落障碍物。

## 项目特点

- ✅ 跨平台支持 (Windows/macOS/Linux)
- ✅ 模块化代码结构
- ✅ 完整碰撞检测
- ✅ 分数系统
- ✅ 游戏状态管理
- ✅ 资源管理器

## 开发环境

- Windows 10/11
- CMake 3.16+
- SFML 3.0.2
- VS Code + CMake Tools

## 构建步骤

### 1. 生成构建文件
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release