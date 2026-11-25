# 运行与部署

## 1. 环境准备

- Windows 7 / 10 x64；
- Visual Studio 2019（Desktop development with C++，`v142` 工具集，Windows SDK 10.0.19041.0）；
- Qt 5.12.0 `msvc2017_64` + Qt VS Tools（工程 `QtInstall` 默认值）；
- 电机驱动器厂商提供的 `vsmdlib.dll` / `vsmdlib.lib`（本仓库仅含 `vsmdlib.h`）；
- RS485/串口转接硬件与已上电的步进驱动器，串口号如 `COM3`。

## 2. 编译

1. 用 Visual Studio 2019 打开 `src/motor_gui.sln`；解决方案含 `Debug|x64` 与 `Release|x64`。
2. 确认 Qt VS Tools 识别到本机 Qt 版本；`src/motor_gui/motor_gui.vcxproj` 中的
   `<AdditionalIncludeDirectories>` 已改为 `$(QTDIR)\include;...` 占位，
   `<QtInstall>` 为 `5.12.0_msvc2017_64`（Release 配置历史值为 `msvc2015_64`），
   本机版本不同时请在 Qt Project Settings 中调整。
3. 将厂商 `vsmdlib.lib` 放到工程链接器可搜索目录（或工程目录），
   `motor_gui.cpp` 使用 `#pragma comment(lib, "vsmdlib.lib")`。
4. 选择 `Debug | x64`，生成解决方案，输出 `motor_gui.exe`。
5. 把 `vsmdlib.dll` 与 `resources/ico/` 复制到 exe 同目录。

> 源码 `.cpp/.h` 含中文字符串与历史注释；`.ui/.sln/.vcxproj/.qrc` 为 UTF-8。
> 请勿整体转码，详见 [注意事项](06-notes.md)。

## 3. 配置

首次运行前创建 `D:\motor_control.ini`（最小示例见 [使用示例](05-usage.md)）。
若文件缺失，Win32 INI API 使用代码内默认值（如 `com_waite=1`、`Com_Port=1`、
`Scan_time=30`、`gear_ratio=0`），但界面参数可能无法按预期保存。

## 4. 运行

1. 连接电机控制器与串口，确认设备管理器中的 COM 号；
2. 双击 `motor_gui.exe`：`main()` 先做单实例检查，随后调用 `time_counter()` 进入倒计时与
   串口探测循环（这是阻塞式主流程，`a.exec()` 在其返回后进入事件循环）；
3. 在界面选择串口 `comboBox` 与齿轮比 `comboBox_gear`，必要时用 `left`/`right` 调整手动角；
4. 点击「寻北」（`pushButton2` → `search_north()`），等待零位状态 `0x180`；
5. 点击「扫描」（`pushButton1` → `time_counter()` → `scan()`），观察 `dial`/`dial_2` 与进度条；
6. 点击「停止」（`pushButton3` → `stop_motor()`）安全停机；托盘图标双击可还原窗口。

## 5. 部署打包（Windows）

```bat
windeployqt motor_gui.exe --release
```

将以下内容一并分发：`motor_gui.exe`、厂商 `vsmdlib.dll`、`resources/ico/` 目录、
`motor_control.ini` 模板，以及上游采集程序约定的 `WindSetting.ini` 写入权限。
程序固定从 `D:\` 读取/写入配置与日志，目标机器需具备相应目录写权限。
