# 注意事项与已知限制

## 1. 源码编码

- `.cpp/.h` 含中文字符串（如窗口标题「测风电机控制」）与历史注释，部分中文键名在源码中
  已退化为 `????`，系历史编码丢失所致，功能依赖的是整型默认值而非文字本身；
- `.ui/.sln/.vcxproj/.qrc` 为 UTF-8；`motor_gui.vcxproj` 使用 `#pragma execution_character_set("utf-8")`
  配合界面字符串；
- 修改时请保持原编码，整体转码可能引入编译告警或界面文字异常。

## 2. 固定路径常量

以下路径是程序约定常量，已按 SPEC 保留原样并在文档中说明；部署时需保证目标机器对应目录可写：

| 路径 | 用途 | 位置 |
|------|------|------|
| `D:\motor_control.ini` | 串口、齿轮比、驻留时间、手动角、速度/电流 | `motor_gui.cpp` 顶部 `lpPath` / `setting_Path_str` |
| `D:\LogFile\YYYY-MM-DD.log` | 运行日志 | `getLogFileName()` |
| `C:\WindSetting.ini` | 与测风采集程序联动（`WHAngle` / `WorkMode` / `WVAngle`） | `SetInt()` / `SetFloat()` / `send_direction()` |
| `HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Run\Motor_start` | 开机自启 | `AutoPowerOn()` / `CanclePowerOn()` |
| `<exe目录>\ico\logo.ico` | 窗口与托盘图标 | `read_ico_path()` + 构造函数 |

发布包中已清理的本机绝对路径：`logo.qrc` 中的个人桌面路径（改为 `felix_logo.jpg`）、
`motor_gui.qrc` 中的 `D:\felix程天一项目\...\logo.ico`（改为 `../logo.ico`）、
`motor_gui.vcxproj` 中的 `D:\software\QT512\...`（改为 `$(QTDIR)` 占位）。

## 3. 第三方库与硬件

- 仓库**不含** `vsmdlib.dll` / `vsmdlib.lib`，只含接口头文件；无对应驱动库与硬件时程序无法
  控制电机，`open()` 会失败并进入重连循环。
- 齿轮比为机械参数：`gear_ratio_switch()` 的 `-6.0 / 2.0 / 1.75` 必须与实际减速比一致，
  否则方位角显示与真实指向不符；单圈脉冲数按 `51200 × ratio` 计算。
- 事件名已脱敏为 `FELIX_WIND_TRACER_20180818`。上游采集程序若仍监听旧名称，需同步修改。

## 4. 运行行为限制

- `main()` 在 `a.exec()` 之前同步调用 `time_counter()`，其内含无限重连循环；
  `scan()` 内也是 `while(1)` 结构，退出依赖 `stop` / `stop_scan` 全局标志与 `qApp->processEvents()`；
- `closeEvent()` 在 `close_window == false`（扫描中）时 `event->ignore()`，需先点「停止」再关闭窗口；
- 进度条由独立 `QThread` 中的 `thread_obj::process_bar_move()` 驱动，
  通过 `start_process_bar` 布尔闸门与主线程协作，非 Qt 线程安全队列连接，修改时需谨慎；
- 累计 3500 次过北后执行 `org()` 重新寻北（`clear_org` 分支），这是有意的周期性回零；
- `readSettingFloat_new()` 为未完成的空实现（仅声明局部变量未初始化即返回），未在主流程调用，
  请勿直接使用。

## 5. 资源文件

- `src/logo.qrc` 为历史资源描述，引用 `felix_logo.jpg`；发布包提供 1×1 占位图，
  可用自有 Logo 替换（保持同名即可）；
- `src/legacy/` 中的 `motor_cp.cpp` 是 `motor.ui` 的 uic 生成代码，重新编译 UI 会被覆盖，
  请勿手工修改。
