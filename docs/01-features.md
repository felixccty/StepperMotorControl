# 功能说明

本文档逐项说明 `StepperMotorControl` 的实际功能，函数名与源码一致。

## 1. 启动与连接管理

| 功能 | 关键函数 | 说明 |
|------|----------|------|
| 单实例保护 | `checkOneRunning()`（`main.cpp`） | `CreateMutex(L"motor_gui")`，已存在实例则直接退出 |
| 启动倒计时 | `motor_gui::time_counter()` | 读取 `[Setting] com_waite`（默认 1），逐秒刷新 `label_5`，期间 `qApp->processEvents()` 保持界面响应 |
| 串口探测 | `time_counter()` 内循环 | 由 `comboBox` 拼出 `\\.\COMn`，`Vsmd::createVsmd(com, 9600)` → `createDevice(0)` → `open()` |
| 断线重连 | 同上 | 连接失败累加 `reconnect_counter`，关闭按钮置灰，`Sleep(100)` 后重试 |
| 线程化入口 | `run_thread()` | 将 `time_counter()` 移入 `QThread` 的可选路径（当前 `scan()` 中调用被注释） |

## 2. 寻北归零 `search_north()`

- `send_model_id(0)`：向 `[WindSetting] WorkMode` 写 0（非扫描态）；
- 从 `comboBox_gear` 当前项取齿轮比 ID，`gear_ratio_switch()` 映射：
  `0 → -6.0`、`1 → 2.0`、`2 → 1.75`、其它 `→ 2.0`；结果写入 `[Setting] gear_ratio`；
- 保存 `Com_Port`、`Scan_time`、`manual_turn` 到 `D:\motor_control.ini`；
- `cfgMcs(8)`、`cfgCurrent(...)`、`enable()`、`cfgSpd(100000)`、`cfgAcc/cfgDec(1000000)`；
- 依 `Slider` 取值调用 `cfgZero(1, 0, 0|1, 60000, 0)`，随后 `zeroStart()`；
- 轮询 `getStatus()`，出现 `0x180` 即认为找到零位：`zeroStop()` → `org()` →
  `moveto(manual_turn * 142)` → 等待 `0x80` → `org()` / `disable()` / `close()`；
- 超过 `over_time == 200`（约 20 s）仍未找到则超时退出；`only_search_north` 控制窗口关闭许可。

## 3. 四方位扫描 `scan()`

- `AutoPowerOn()` 写入注册表自启项 `Motor_start`；随后 `search_north()` + `load_speed()`；
- 外循环每次 `send_model_id(4)` 设 `WorkMode=4`（VAD 模式）；
- `read_voltage()` 读取并钳位电流到 `[1.7, 2.5]` A：`acc_cur = run_cur = votage_set`，
  `ava_cur = votage_set - 0.5`；
- `cfgCurrent(acc_cur, run_cur, ava_cur)`、`cfgSpd(run_speed)`、`cfgAcc/cfgDec(accelerate)`、`org()`；
- 内循环每 40 ms 轮询：`isOnline()` 判离线；`getStatus() & 0x01` 控制传感器按钮颜色；
- 到达 0°/90°/180°/270° 且 `move_deg > 30` 时高亮对应方向按钮，调用 `send_direction()`：
  写 `[WindSetting] WHAngle = 方位角`，并 `SetEvent(FELIX_WIND_TRACER_20180818)` 通知采集端；
- 驻留 `doubleSpinBox_wait` 秒期间由 `thread_obj` 发射 `bar_signal(0..100)` 驱动进度条；
- `moveto(position + position_move)`，其中 `position_move = 12800 * gear_ratio_float`；
- 每累计 3500 次过北，`org()` / `disable()` / `close()` 并置 `clear_org`，重新 `search_north()`；
- `stop_motor()` 将全局 `stop` / `stop_scan` 置真，安全停机并恢复按钮状态。

## 4. 角度换算与显示

- `getCurPos()` → `motor_dial_turn = ((pos % (51200 * ratio)) * 9) / 1280`；
- `radar_dial_turn = motor_dial_turn / ratio + 180`；
- `show_deg` 在 88–92、178–182、268–272、350–360 区间做吸附，消除抖动；
- 更新 `dial`、`dial_2`（电机表盘）与 `label_3`（角度文字）、`label_11`（当前脉冲位置）。

## 5. 参数配置

| 功能 | 关键函数 | 存储 |
|------|----------|------|
| 电流读取/钳位 | `read_voltage()` | `[Setting]` @ `D:\motor_control.ini` |
| 速度/加速度读取 | `load_speed()` | `GetPrivateProfileInt` + `doubleSpinBox_speed/acc` |
| 速度写回 | `write_speed()` | `WritePrivateProfileString`（spinbox 变化触发） |
| 字符串读写 | `readSettingString()` / `writeSettingString()` | `QSettings` UTF-8 INI |
| 手动角度 | `left_move()` / `right_move()` | `manual_turn` 增减 1，`degree_of_move()` 显示 ±0.5° 步进 |
| 日志 | `writeLog()` / `getLogFileName()` | `D:\LogFile\YYYY-MM-DD.log`，5 级日志 |

## 6. 界面与系统联动

- `motor_gui()` 构造函数：加载 `:/styles/motor_gui_theme.qss`，设置托盘图标与提示，
  恢复串口、齿轮比、驻留时间、手动角等 UI 状态，连接全部按钮信号槽；
- `MotorGuiUiTheme::apply()`：微软雅黑 UI 9pt 字体、标题「测风电机控制」、最小 780×500、
  `label_5` 透明度呼吸动画、进度条 `%p%` 格式；
- `iconActivated()`：托盘 Trigger → `showNormal()`；`closeEvent()`：扫描中忽略关闭；
- `CanclePowerOn()`：删除注册表自启项（预留接口）。
