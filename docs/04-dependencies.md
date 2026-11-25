# 依赖配置

## 编译依赖

| 类别 | 版本 / 组件 | 说明 |
|------|-------------|------|
| 语言 | C++（MSVC） | Debug 为 `v142`，Release 历史配置 `v140` |
| IDE | Visual Studio 2019 | Desktop C++ 工作负载 + Windows SDK 10.0.19041.0 |
| Qt | 5.12.0 `msvc2017_64` | 仅需 `QtCore` / `QtGui` / `QtWidgets`（`QtModules=core;gui;widgets`） |
| Qt VS Tools | 兼容 VS2019 的版本 | 解析 `.vcxproj` 中的 `QtInstall` / `QtMoc` / `QtUic` / `QtRcc` |
| 预编译头 | `StdAfx.h/.cpp` | 工程启用预编译头 |

## 运行时依赖

| 类别 | 说明 |
|------|------|
| VSMD 驱动库 | `vsmdlib.dll` / `vsmdlib.lib` 由步进驱动器厂商提供；仓库只保留接口 `src/motor_gui/vsmdlib.h`。程序以 9600 bps 通过 `\\.\COMn` 访问设备 |
| Qt 运行库 | 用 `windeployqt` 收集 `Qt5Core.dll` / `Qt5Gui.dll` / `Qt5Widgets.dll` 及平台插件 |
| 硬件 | VSMD 系列步进驱动器、步进电机与测风雷达转台、RS485 转串口模块 |
| 上游软件 | 读取 `C:\WindSetting.ini` 的 `WHAngle` / `WorkMode`，并监听事件 `FELIX_WIND_TRACER_20180818` 的测风采集程序 |

## 第三方 SDK 获取

`vsmdlib.h` 中声明了 `Vsmd` / `VsmdDevice` 两个导出类（`VSMD_DLL_API`），包含
`createVsmd`、`createDevice`、`open/close`、`cfgCurrent`、`cfgSpd/Acc/Dec`、
`cfgZero`、`zeroStart/Stop`、`org`、`moveto`、`getCurPos`、`getStatus` 等接口。
本仓库不包含其实现二进制；请根据实际电机型号向驱动器供应商索取配套 DLL/LIB，
并确认接口版本与头文件一致。

## 系统要求

- Windows 7 / 10 x64；
- 对 `D:\` 与 `C:\` 根目录具备读写权限（配置、日志与联动文件均为固定路径）；
- 串口权限可用，建议关闭占用同一 COM 口的其它程序；
- 建议 2 GB 以上内存；扫描循环通过 `qApp->processEvents()` 保持界面响应，
  但更新频率为 40 ms 轮询，低配机器可能出现表盘刷新滞后。
