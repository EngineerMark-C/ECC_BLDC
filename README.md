# Extreme Cross Country BLDC

基于 **Infineon Aurix TC264D (TriCore)** 的自主导航小车项目，支持 GPS/INS 多模态导航、BLDC 电机驱动、WiFi 遥控及语音识别。

---

## 项目简介
本项目在备赛第二十届全国大学生智能汽车竞赛中，基于逐飞库开发。

由于现场种种意外以及备赛不充分，最后仅获得华北赛区一等奖，未能进国。

现将代码开源于此，往后来者有所启发。

关于组合导航部分，可以看看[一种INS+GPS实用组合导航方法](https://www.engmarkc.com/articles/Practical_INS_GPS_Integrated_Navigation_Method/)

## 硬件平台

| 组件 | 型号/规格 |
|------|-----------|
| **主控 MCU** | Infineon Aurix TC264D (TriCore 双核) |
| **电机驱动** | BLDC 无刷直流电机 (ATOM0_CH4 PWM 输出) |
| **转向** | 舵机 (ATOM3_CH3 PWM, 转角范围 ±30°) |
| **编码器** | 带方向编码器 (TIM5, 齿轮比 27:110) |
| **IMU** | ICM42688 / IMU963RA (SPI, 200Hz 采样) |
| **GPS** | TAU1201 GNSS 模组 |
| **摄像头** | MT9V03X (灰度摄像头, 用于白线检测) |
| **显示屏** | IPS 1.14" TFT 彩屏 (SPI2) |
| **LED 驱动** | Infineon TLD7002 (16 通道) |
| **拨码开关** | 4 位 DIP 开关 (P22.0 - P22.3) |
| **Flash 存储** | 板载 Flash，用于存储航点和参数 |
| **语音识别** | 讯飞 ASR (WebSocket, 通过 WiFi) |

## 软件架构

```
├── code/
│   ├── device/             # 硬件驱动层
│   │   ├── bldc.c/h        # BLDC 无刷电机驱动
│   │   ├── steer.c/h       # 舵机驱动
│   │   ├── encoder.c/h     # 编码器测速
│   │   ├── icm42688.c/h    # ICM42688 IMU 驱动 (SPI/IIC)
│   │   ├── imu.c/h         # IMU 姿态解算 (Mahony/Madgwick)
│   │   ├── gps.c/h         # GPS (TAU1201) 数据解析
│   │   ├── camera.c/h      # MT9V03X 摄像头驱动
│   │   ├── ips.c/h         # IPS 114 显示屏驱动
│   │   ├── flash.c/h       # Flash 存储管理
│   │   ├── wifi.c/h        # WiFi UDP 通信与指令解析
│   │   ├── switch.c/h      # 拨码开关读取
│   │   ├── uartReceiver.c/h # UART 接收器
│   │   ├── TLD7002_driver/ # Infineon TLD7002 LED 驱动
│   │   ├── zf_device_dot_matrix_screen.c/h  # 点阵屏驱动
│   │   ├── init.c/h        # 硬件初始化入口
│   │   └── xf_asr/         # 讯飞语音识别 (WebSocket + SHA1 + Base64)
│   │
│   └── software/           # 应用算法层
│       ├── control.c/h     # 导航控制 (航点处理 / 坐标系转换 / 速度管理)
│       ├── pid.c/h         # PID 控制器 (位置式 / 增量式, 参数查找表)
│       ├── ins.c/h         # 惯性导航系统 (编码器航位推算)
│       ├── mode.c/h        # 导航模式切换 (GPS/ENU/INS/组合)
│       ├── boundary.c/h    # 安全边界计算与检查
│       ├── menu.c/h        # 多级菜单系统 (按键交互)
│       ├── pathshow.c/h    # 路径可视化 (IPS 屏幕绘制)
│       ├── image.c/h       # 图像处理
│       ├── test.c/h        # 科目优化
│       └── test4.c/h       # 科目四
│
├── user/                   # 用户应用层
│   ├── cpu0_main.c/h       # CPU0 主程序
│   ├── cpu1_main.c         # CPU1 主程序
│   ├── isr.c/h             # 中断服务程序
│   └── isr_config.h        # 中断配置
│
├── libraries/              # 逐飞科技 TC264 开源库 & Infineon iLLD
│   └── infineon_libraries/ # Infineon 底层驱动 (SFR 寄存器定义等)
│
└── flows.json              # Node-RED 遥控仪表盘配置文件
```

## 导航模式

系统支持以下五种导航模式，可通过拨码开关或菜单切换：

| 模式 | 说明 |
|------|------|
| **GPS 导航** | 纯 GPS 原始经纬度导航 |
| **GPS ENU 导航** | GPS 转 ENU 局部坐标系导航 |
| **INS 导航** | 纯惯性导航 (编码器 + IMU 航位推算) |
| **GPS + INS 导航** | GPS 与 INS 组合导航 |
| **GPS + ENU + INS 导航** | 全组合导航 |

- **航点容量**: 最多 32 个 GPS 航点 + 32 个 INS 航点 + 32 个 S 型走位点
- **坐标系转换**: WGS84 → ENU → 导航坐标系
- **发车方向**: 双点发车方向检测
- **镜像航点**: 支持航点镜像生成 (往返)
- **S 型走位**: 自动生成 S 型路径航点

## 控制算法

### 双 PID 闭环控制

- **速度 PID**: 编码器反馈 → PID 计算 → BLDC PWM 输出
  - 支持多组 PID 参数自动切换（按目标速度分段）
  - 位置式 PID，带积分分离
- **舵机 PID**: IMU 角度反馈 → PID 计算 → 舵机角度输出
  - 支持多组 PID 参数自动切换（按当前速度分段）

### PID 参数表

```
速度 MaxSpeed 5.5  → P=550,  I=2800, D=0
速度 MaxSpeed 12.0 → P=520,  I=3200, D=0
速度 MaxSpeed 16.0 → P=560,  I=4300, D=0
```

### 速度管理

- 根据与目标点的距离自动调整速度（加速段 / 巡航段 / 减速段）
- GPS/INS 切换距离可配置
- 安全边界检查，防止越界

## Node-RED 遥控系统

`flows.json` 提供了完整的 Node-RED 仪表盘配置，功能包括：

| 功能 | 指令格式 |
|------|----------|
| 前进/后退/左转/右转 | 字符串指令 |
| 停止 | `stop` |
| 速度控制 | `speed:<value>` (-800 ~ 800) |
| 角度控制 | `angle:<value>` (655 ~ 885) |
| 连接测试 | `ping` |
| 状态回传 | `status:speed=<v>,angle=<a>` |

- 通信协议: UDP
- 发送端口: 8080 (小车端)
- 接收端口: 1111 (Node-RED 端)
- 小车 IP: 192.168.1.26

## IPS 显示屏菜单

系统内置多级菜单，通过板载按键操作，支持以下功能：

- 主菜单 (状态概览)
- 舵机调试菜单
- GPS 信息 / GPS 航点 / ENU 航点显示
- INS 航点管理
- S 型走位点管理
- 摄像头 (白线检测) 显示
- 路径可视化 (GPS / ENU / INS / S 路径)
- 安全边界显示
- 导航模式选择
- 速度参数管理
- 陀螺仪校准
- 发车方向设置
- 语音 & LED 控制
- 测试模式 & 科目管理

## 项目状态

- [x] BLDC 电机驱动
- [x] 舵机转向控制
- [x] 编码器测速
- [x] IMU 姿态估计
- [x] GPS 定位与航点导航
- [x] INS 惯性导航
- [x] 多模态组合导航
- [x] WiFi 远程遥控
- [x] Node-RED 仪表盘
- [x] IPS 屏多级菜单
- [x] 路径可视化
- [x] Flash 航点存储
- [x] 安全边界检查
- [x] 讯飞语音识别
- [ ] 摄像头循线 (未完成)

---

*Built with Infineon Aurix TC264D + SEEKFREE Open Source Library*
