# Changelog — ADRC速度环替换

## 变更概要
将原有位置式PI速度环（`PID_L_pos` / `PID_R_pos`）替换为一阶线性ADRC（`ADRC_L` / `ADRC_R`），采用保守方案A参数。方向环PID、增量式PID等其他控制器保持不变。

---

## 新增文件

### `project/code/ardc.h`
- **内容**：一阶线性ADRC控制器结构体 `ADRC_Controller` 及函数声明
- **接口**：
  - `ADRC_Controller`：含 z1（状态估计）、z2（扰动估计）、b0、beta1、beta2、kp
  - `extern ADRC_Controller adrc_L / adrc_R`：左右轮ADRC实例
  - `int16 ADRC_L(void)` / `int16 ADRC_R(void)`：速度环计算函数

### `project/code/ardc.c`
- **内容**：ADRC控制器实现
- **参数**（保守方案A，100Hz采样）：
  - `b0 = 8.0f`：控制增益估计
  - `beta1 = 188.4f`（= 2ω_o）：ESO增益1
  - `beta2 = 8874.0f`（= ω_o²）：ESO增益2
  - `kp = 31.4f`（= ω_c）：控制器增益
- **算法**：
  1. ESO更新：`z1 += T*(z2 - beta1*e + b0*u_actual)`，`z2 += T*(-beta2*e)`
  2. 控制律：`u0 = kp*(r - z1)`，`u = (u0 - z2)/b0`
  3. 死区前馈 + 输出限幅（沿用原 `MOTOR_DEAD_ZONE`、`MAX_SPD_OUT`）

---

## 修改文件

### `project/code/motor.c`
- **包含头文件**：新增 `#include "ardc.h"`
- **函数 `motor_control()`**：
  - **原调用**：`speed_outL = PID_L_pos(); speed_outR = PID_R_pos();`
  - **新调用**：`speed_outL = ADRC_L(); speed_outR = ADRC_R();`
- **接口语义不变**：
  - 输入：全局变量 `target_speed_L/R`、`real_speed_L/R`
  - 输出：返回值范围 `[-MAX_SPD_OUT, MAX_SPD_OUT]`，符号表示方向
  - 后续DIR/PWM设置逻辑完全不变

---

## 未改动文件（保留原有代码）

| 文件 | 说明 |
|------|------|
| `project/code/pid.h` | 原PID头文件未修改，所有PID声明保留 |
| `project/code/pid.c` | 原PID实现未修改，`PID_track`、`PID_L`、`PID_R`、`PID_L_pos`、`PID_R_pos` 全部保留 |
| `project/code/config.h` | 未新增 `ardc.h` 包含，由 `motor.c` 直接包含 |
| `project/user/main.c` | 未修改，`KP_v`/`KI_v` 初始化保留（如需回退到PID，改 `motor.c` 调用即可）|
| `project/code/menu.c` | 未修改，调参菜单仍操作 `KP_v`/`KI_v`（ADRC参数暂未接入菜单）|

---

## 等效接口说明

| 项目 | 原PI | 新ADRC | 备注 |
|------|------|--------|------|
| 速度环入口 | `PID_L_pos()` / `PID_R_pos()` | `ADRC_L()` / `ADRC_R()` | 返回值语义相同 |
| 目标速度输入 | `target_speed_L/R`（全局） | 同左 | 未变 |
| 实际速度输入 | `real_speed_L/R`（全局） | 同左 | 未变 |
| 控制输出 | 返回值 + `PID_outL/R` | 返回值 + `PID_outL/R` | ADRC复用PID_outL/R存储实际控制量 |
| 死区前馈 | `MOTOR_DEAD_ZONE` | 同左 | 沿用 |
| 输出限幅 | `MAX_SPD_OUT` | 同左 | 沿用 |
| 方向切换处理 | 清积分 | **无需重置**（ESO自动跟踪扰动变化） | ADRC原生优势，无需额外逻辑 |

---

## 回退方法

如需回退到原有位置式PI，只需修改 `project/code/motor.c` 中 `motor_control()` 的两行调用：

```c
speed_outL = PID_L_pos();   // 改回PI
speed_outR = PID_R_pos();   // 改回PI
```

并移除 `#include "ardc.h"`（可选）。

---

## 后续建议

1. **b0在线标定**：新增调试模式，固定PWM输出2000，测量速度响应斜率，自动标定b0
2. **ADRC参数调参**：将 `adrc_ctrl_L.b0`、`adrc_ctrl_L.kp` 等接入 `menu.c` 调参菜单
3. **采样率提升**：若需更高带宽，将 `pit_ms_init(PIT_SP, 10)` 改为 `pit_ms_init(PIT_SP, 2)`，并同步调整 `beta1`、`beta2`、`kp`（参见ARDC_plan.md方案B）

---

## 2025-06-01 编译修复

### 修复内容

1. **`project/mdk/Intelligent.uvproj`**
   - 将 `project/code/ardc.c`、`project/code/ardc.h` 加入项目 `code` 文件组，解决链接错误 `L127: UNRESOLVED EXTERNAL SYMBOL (ADRC_L / ADRC_R)`

2. **`project/code/ardc.c` / `project/code/ardc.h`**
   - 重命名全局变量 `adrc_L` → `adrc_ctrl_L`、`adrc_R` → `adrc_ctrl_R`
   - **原因**：Keil C251 编译器/链接器**默认大小写不敏感**，导致变量 `adrc_L` 与函数 `ADRC_L` 被视为同一符号，产生链接错误 `L104: MULTIPLE PUBLIC DEFINITIONS`

### 编译结果

- 编译错误（`error Cxxx`）：**0 个**
- 链接错误（`ERROR Lxxx`）：仅剩 `L250`（Keil C251 评估版代码大小限制，与项目代码无关）
