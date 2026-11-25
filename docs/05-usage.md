# 使用示例

## 1. 配置文件 `D:\motor_control.ini`

程序用 Win32 `GetPrivateProfileInt` / `WritePrivateProfileString` 及 `QSettings` 读写 `[Setting]` 段。
以下为源码中可确认的键（中文键名在历史源码中已退化为 `????`，以界面控件与默认值为准）：

```ini
[Setting]
com_waite=1          ; 启动等待秒数，默认 1（time_counter 倒计时）
Com_Port=3           ; 串口号，对应 comboBox 当前项 + 1
gear_ratio=0         ; 齿轮比索引：0=-6.0, 1=2.0, 2=1.75
Scan_time=30         ; 方位驻留时间 ×10（即 3.0 秒），对应 doubleSpinBox_wait
manual_turn=0        ; 手动角偏移步数，每步 0.5°（left/right 修改）
```

> 速度、加速时间与电流设定键由 `load_speed()` / `read_voltage()` / `write_speed()` 读写，
> 默认值分别为 `100000`、`120000`、电流 `2.3 A`（钳位 1.7–2.5 A）。
> 实际键名请以当前源码中 `GetPrivateProfileInt` 的宽字符串为准；未配置的键使用默认值。

## 2. 联动文件 `C:\WindSetting.ini`

由 `SetInt()` / `SetFloat()` 写入 `[WindSetting]` 段，供上游采集程序读取：

```ini
[WindSetting]
WorkMode=4      ; send_model_id(4)：VAD 扫描模式；寻北时为 0
WHAngle=90      ; send_direction()：当前方位角 0/90/180/270
WVAngle=75      ; 构造函数中固定写入 75
```

每次到达方位点，`send_direction()` 还会对具名事件
`FELIX_WIND_TRACER_20180818` 调用 `SetEvent()`。事件名已在发布包中脱敏为 FELIX 前缀，
若上游采集程序仍使用旧事件名，需同步更新双方配置。

## 3. 日志格式

日志按日写入 `D:\LogFile\YYYY-MM-DD.log`，格式为：

```text
2026-11-25 10:31:02 : [Info] 方位角:90
2026-11-25 10:31:05 : [Info] 3500圈到位，重新寻北
```

级别定义见 `motor_gui.cpp` 中 `LogLevel`（Debug / Info / Error / Warn / Fatal）。

## 4. 典型操作流程

1. 上电并检查驱动器、串口连线；启动 `motor_gui.exe`。
2. 在 `comboBox` 选择 COM 口；在 `comboBox_gear` 选择传动比（1:6 / 1:2 / 1:1.75）。
3. 用 `doubleSpinBox_speed` / `doubleSpinBox_acc` 设定速度与加速度，
   `doubleSpinBox_wait` 设定每方位驻留秒数；滑块的 0 / 非 0 决定寻北零位方向参数。
4. 点击「寻北」：`search_north()` 执行 `zeroStart()`，等待状态 `0x180`，
   完成零位与 `manual_turn * 142` 的手动补偿。
5. 点击「扫描」：`scan()` 启动四方位循环；每到一位写入 `WHAngle` 并触发事件，
   `progressBar` 显示驻留进度，`dial` 显示雷达方位角。
6. 需要微调角度时用 `left` / `right`（每次 ±0.5°），结果写入 `manual_turn`。
7. 点击「停止」：`stop_motor()` 置位停止标志，执行 `org()` / `disable()` 并恢复按钮。

## 5. 角度换算示例

齿轮比 1.75 时：`position_move = 12800 × 1.75 = 22400` 脉冲/步；
一圈脉冲数 `51200 × 1.75 = 89600`，恰好对应雷达 360°。
`getCurPos()` 每 40 ms 读取一次，经
`motor_dial_turn = ((pos % 89600) × 9) / 1280`、`radar_dial_turn = motor_dial_turn / 1.75 + 180`
换算后更新表盘；接近四个正交方位时做 ±2° 吸附以抑制抖动。
