# StepperMotorControl

> 基于 Qt 5 / C++ 的步进电机运动控制上位机，驱动测风雷达转台完成寻北归零、四方位扫描与角度同步。

[![Platform](https://img.shields.io/badge/platform-Windows%20x64-blue.svg)](https://www.microsoft.com/windows)
[![Language](https://img.shields.io/badge/language-C%2B%2B-orange.svg)](https://isocpp.org/)
[![Framework](https://img.shields.io/badge/framework-Qt%205-green.svg)](https://www.qt.io/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

`StepperMotorControl` 是一套面向激光测风雷达转台的**电机控制与扫描调度上位机**。软件通过
VSMD 系列步进驱动器（RS485 / 9600 bps）控制电机，先执行寻北归零，再按 0°、90°、180°、270°
四个方位循环驻留扫描；每到一位便把方位角写入 `C:\WindSetting.ini`，并通过具名 Win32 事件
通知上游测风采集程序，实现雷达指向与数据采集的同步。

界面由 Qt Designer `.ui` 生成，并叠加深色工业风 QSS 主题、系统托盘与双表盘（雷达角 / 电机角）
实时显示；运行参数通过 `D:\motor_control.ini` 持久化，运行日志按日写入 `D:\LogFile\`。

---

## 核心特性

- **单实例运行**：`checkOneRunning()` 用 `CreateMutex(L"motor_gui")` 防止重复启动。
- **串口自动重连**：`time_counter()` 读取 `com_waite` 倒计时，按 `\\.\COMn` @ 9600 bps
  探测 VSMD 设备，失败自动计数重连。
- **寻北归零**：`search_north()` 配置 `cfgZero` / `zeroStart()`，等待零位状态 `0x180`，
  并按 `manual_turn` 做手动角偏移。
- **四方位扫描**：`scan()` 每圈依次经过北/东/南/西，高亮方向按钮并调用 `send_direction()`
  写入 `WHAngle` + `SetEvent`；累计 3500 次过北后自动回零重启。
- **多齿轮比切换**：`gear_ratio_switch()` 支持 `-6.0`、`2.0`、`1.75` 三种传动比，
  `position_move = 12800 × ratio` 决定单次步进量（`gear_ratio_float`）。
- **电流与速度配置**：`read_voltage()` 将运行/加速电流限制在 1.7–2.5 A；
  `load_speed()` / `write_speed()` 通过 `QSettings` 与 `GetPrivateProfileInt` 读写参数。
- **实时反馈**：`getCurPos()` 经编码器换算得到雷达方位角，更新 `dial` / `dial_2` 与角度标签；
  `thread_obj::process_bar_move()` 在独立 `QThread` 中驱动驻留进度条。
- **联动与自启**：`send_model_id(4)` 写入 `WorkMode=4`（VAD 模式）；`AutoPowerOn()` 通过
  注册表 `HKCU\...\Run\Motor_start` 设置开机自启；`closeEvent()` 在运行时阻止误关闭。
- **托盘与主题**：`QSystemTrayIcon` 双击还原窗口；`MotorGuiUiTheme::apply()` 注入 QSS 主题、
  状态栏呼吸动画与进度格式。

---

## 技术栈

| 类别 | 选型 |
|------|------|
| 语言 | C++（MSVC `v142` / 历史 `v140` 工具集） |
| UI 框架 | Qt 5.12（Core / Gui / Widgets）+ Qt VS Tools |
| 开发工具 | Visual Studio 2019（x64） |
| 电机驱动 | VSMD 系列（`vsmdlib.h` 接口声明，`vsmdlib.dll/.lib` 需自备） |
| 通信 | RS485 / 串口 9600 bps（`\\.\COMn`） |
| 配置存储 | `D:\motor_control.ini`（Win32 INI API + `QSettings`） |
| 外部联动 | `C:\WindSetting.ini`、具名事件 `FELIX_WIND_TRACER_20180818`、注册表 Run 键 |
| 操作系统 | Windows 7 / 10 x64 |

---

## 快速开始

```bash
git clone https://github.com/felixccty/StepperMotorControl.git
cd StepperMotorControl
```

1. 安装 Visual Studio 2019（C++ 桌面负载）与 Qt 5.12 msvc2017_64 + Qt VS Tools。
2. 自备 VSMD 官方 `vsmdlib.dll` 与 `vsmdlib.lib`，放到可执行文件同目录（本仓库仅含接口头文件）。
3. 用 VS 打开 `src/motor_gui.sln`，选 `Debug | x64` 编译；`vcxproj` 中 Qt 路径已改为
   `$(QTDIR)` 占位，若本机 Qt 版本不同请调整 `QtInstall` 设置。
4. 将 `resources/ico/` 放到 exe 同目录（程序按 `<exe目录>/ico/logo.ico` 读取图标）。
5. 准备 `D:\motor_control.ini`（字段见 [使用示例](docs/05-usage.md)）并连接电机控制器。
6. 运行 `motor_gui.exe`：选择串口与齿轮比 → 点击「寻北」→ 点击「扫描」。

完整步骤见 [运行与部署](docs/03-build-and-run.md) 与 [依赖配置](docs/04-dependencies.md)。

---

## 目录结构

```
StepperMotorControl/
├── README.md / LICENSE / CHANGELOG.md / CONTRIBUTING.md / .gitignore
├── src/                           # 核心源码（保持原编码）
│   ├── main.cpp                   # 入口：单实例检查、QApplication、启动 time_counter
│   ├── motor_gui.sln              # VS2019 解决方案
│   ├── logo.qrc / logo.ico        # 历史 Logo 资源（felix_logo.jpg 为占位）
│   ├── motor_gui/
│   │   ├── motor_gui.h/.cpp       # 主窗口类与全部业务槽函数
│   │   ├── motor_gui.ui           # Qt Designer 界面
│   │   ├── main.cpp               # 工程内入口副本
│   │   ├── vsmdlib.h              # VSMD 驱动接口声明（第三方）
│   │   ├── motor_gui.qrc          # 图标 / QSS 资源描述
│   │   ├── motor_gui_ui_theme.*   # 深色主题注入
│   │   ├── styles/*.qss           # 界面样式表
│   │   └── motor_gui.vcxproj      # VS 工程（Qt 路径可移植）
│   └── legacy/                    # 早期 Dialog 界面 uic 产物（motor.ui / motor_cp.cpp）
├── resources/ico/                 # 图标资源（logo.ico / felix_logo.jpg）
├── docs/                          # 本文档体系 + images/ 示意图
└── (本地按需) motor_control.ini / WindSetting.ini / vsmdlib.dll/.lib / Qt 运行库
```

---

## 文档索引

| 文档 | 内容 |
|------|------|
| [功能说明](docs/01-features.md) | 寻北、四方位扫描、齿轮比、电流速度配置、进度与联动 |
| [目录结构](docs/02-structure.md) | 源文件职责、关键类成员与数据流 |
| [运行与部署](docs/03-build-and-run.md) | 编译、配置、运行与 windeployqt 部署 |
| [依赖配置](docs/04-dependencies.md) | Qt 模块、VSMD SDK、串口硬件与系统要求 |
| [使用示例](docs/05-usage.md) | INI 配置示例、事件联动与典型操作流程 |
| [注意事项](docs/06-notes.md) | 编码、固定路径、第三方库与已知限制 |
| [架构与流程](docs/07-architecture.md) | 分层架构、4 张示意图与关键时序 |

## 第三方 SDK 说明

本仓库**不含** VSMD 驱动二进制（`vsmdlib.dll` / `vsmdlib.lib`），仅保留公开接口头文件
`src/motor_gui/vsmdlib.h`。请向驱动器厂商获取与硬件匹配的库文件后自行放置。

---

## 许可与作者

- **许可**：[MIT License](LICENSE)
- **作者 / 项目归属**：Felix
- **仓库**：https://github.com/felixccty/StepperMotorControl

本项目以学习和工程交流为目的开源，欢迎 Issue 与 Pull Request。使用前请自行评估其在目标场景下的
适用性与安全性；电机与转台属于机电设备，务必在安全环境下调试。
