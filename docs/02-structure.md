# 目录结构

## 发布包目录

```
StepperMotorControl/
├── README.md / LICENSE / CHANGELOG.md / CONTRIBUTING.md / .gitignore
├── src/
│   ├── main.cpp                    # 程序入口：单实例检查 + 启动 time_counter
│   ├── motor_gui.sln               # VS2019 解决方案（Debug|Release x64）
│   ├── logo.qrc / logo.ico         # 历史 Logo 资源（felix_logo.jpg 为占位）
│   ├── felix_logo.jpg              # qrc 引用的占位图，可替换
│   ├── motor_gui/
│   │   ├── main.cpp                # 工程内入口副本
│   │   ├── motor_gui.h/.cpp        # 主窗口类、槽函数、驱动调用
│   │   ├── motor_gui.ui            # Qt Designer 主界面
│   │   ├── closeBox.ui             # 附加对话框界面
│   │   ├── vsmdlib.h               # VSMD 驱动接口声明（第三方）
│   │   ├── motor_gui.qrc           # logo.ico + styles QSS 资源描述
│   │   ├── motor_gui_ui_theme.h/.cpp  # 主题注入
│   │   ├── styles/motor_gui_theme.qss # 深色工业风样式
│   │   ├── motor_gui.vcxproj(.filters) # VS 工程
│   │   └── StdAfx.cpp/.h, resource*.h  # 预编译头与资源 ID
│   └── legacy/
│       ├── motor.ui                # 早期 Dialog 界面定义
│       ├── motor_cp.cpp            # uic 生成的界面代码（勿手改）
│       └── motor_ui.h              # 对应头文件
├── resources/ico/                  # logo.ico / felix_logo.jpg
├── docs/                           # 本文档体系
│   └── images/                     # 4 张 SVG 示意图
└── (本地按需) motor_control.ini / WindSetting.ini / vsmdlib 库 / Qt 运行库
```

## 源文件职责

| 文件 | 职责 |
|------|------|
| `src/main.cpp` | `checkOneRunning()` 单实例；`QApplication`；`motor_gui w; w.show(); w.time_counter(); a.exec()` |
| `src/motor_gui/motor_gui.h` | `motor_gui : QWidget` 声明：驱动指针 `Vsmd* vsmd`、`VsmdDevice* devices[1]`、电流成员、槽函数；`thread_obj : QObject` 进度条线程 |
| `src/motor_gui/motor_gui.cpp` | 全部业务：日志、INI 读写、`time_counter` / `scan` / `search_north` / `stop_motor` / `closeEvent` / 主题调用与信号连接 |
| `src/motor_gui/motor_gui.ui` | 控件：`pushButton1/2/3`、`left`/`right`、`comboBox`、`comboBox_gear`、`doubleSpinBox_speed/acc/wait`、`Slider`、`dial`/`dial_2`、各 `label`、`progressBar`、方向按钮 |
| `src/motor_gui/vsmdlib.h` | `Vsmd` 工厂 + `VsmdDevice` 虚接口：`cfgMcs/cfgCurrent/cfgSpd/cfgAcc/cfgDec/cfgZero/zeroStart/org/moveto/getCurPos/getStatus` 等 |
| `src/motor_gui/motor_gui.qrc` | `:/styles/motor_gui_theme.qss` 与图标资源（路径已改为相对） |
| `src/motor_gui/motor_gui_ui_theme.*` | `MotorGuiUiTheme::apply()`：字体、QSS、标题、尺寸、动画 |
| `src/motor_gui/styles/*.qss` | 深色工业风控件样式（QWidget / QLabel / QPushButton / QProgressBar / QDial 等） |
| `src/motor_gui/motor_gui.vcxproj` | MSBuild 工程：`v142`（Debug）/ `v140`（Release）、Qt 模块 `core;gui;widgets`、moc/uic/rcc 项 |
| `src/legacy/*` | 早期 dialog 版转台控制界面，保留供追溯 |

## 关键全局变量与状态位

| 名称 | 含义 |
|------|------|
| `stop` / `stop_scan` | 停止扫描 / 停止串口循环 |
| `only_search_north` / `search_N_stop` | 仅寻北模式 / 寻北停止标志 |
| `close_window` | 是否允许窗口关闭（扫描中置 false） |
| `run_speed` / `accelerate` | 运行速度、加减速（脉冲单位，默认 100000 / 120000） |
| `gear_ratio_float` | 当前齿轮比（-6.0 / 2.0 / 1.75） |
| `scan_time` | 方位驻留时间（秒，来自 `doubleSpinBox_wait`） |
| `start_process_bar` | 进度条线程启动闸门 |
| `s_hEnventAquire` | 具名事件句柄（`FELIX_WIND_TRACER_20180818`） |

## 数据流

```
用户操作(按钮/spinbox)
        │ 信号槽
        ▼
motor_gui 槽函数 ──VSMD API──> 步进驱动器 ──RS485──> 电机/转台
        │                                      │
        │ getCurPos/getStatus <─────────────────┘
        ▼
dial/dial_2/label_3/progressBar 实时显示
        │
        ├─ SetInt(WHAngle/WorkMode) ──> C:\WindSetting.ini
        ├─ SetEvent(FELIX_WIND_TRACER_20180818) ──> 采集软件
        ├─ WritePrivateProfileString ──> D:\motor_control.ini
        └─ writeLog ──> D:\LogFile\YYYY-MM-DD.log
```

配置路径 `D:\motor_control.ini`、`C:\WindSetting.ini` 是程序约定常量，已按原样保留，
详见 [注意事项](06-notes.md)。
