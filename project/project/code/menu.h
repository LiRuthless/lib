#ifndef _menu_h_
#define _menu_h_

// 按键值枚举
#define OK      1       // 确定键
#define UP      2       // 上键
#define DOWN    3       // 下键
#define BACK    4       // 返回键
#define LEFT    5       // 左键（参数减）
#define RIGHT   6       // 右键（参数加）
#define RST     7       // 复位键（返回主页）
#define TWO     8       // 预留
#define THREE   9       // 预留
#define FOUR    10      // 预留

// 按键ADC通道定义（使用ADC分压键盘）
#define KEY_CHANNEL1    (ADC_CH4_P14)   // 第一路按键ADC（方向键+确定）
#define KEY_CHANNEL2    (ADC_CH3_P13)   // 第二路按键ADC（复位+返回）

// 页面ID定义（提高可读性，避免魔法数字）
#define PAGE_HOME           0   // 主页
#define PAGE_ADC            1   // ADC实时数据显示
#define PAGE_MODE_SELECT    2   // 模式选择
#define PAGE_PID_TYPE       3   // PID类型选择（方向/速度）
#define PAGE_MODE_STEADY    21  // 平稳模式加载确认
#define PAGE_MODE_ORDINARY  22  // 普通模式加载确认
#define PAGE_MODE_FAST      23  // 快速模式加载确认
#define PAGE_PID_DIR        31  // 方向PID参数修改
#define PAGE_PID_SPEED      32  // 速度PID参数修改

// 运行模式索引
#define MODE_STEADY     0
#define MODE_ORDINARY   1
#define MODE_FAST       2

// 函数声明
void switch_mode(uint8 mode);                       // 切换运行模式（平稳/普通/快速）
void save_current_param(uint8 mode);                // 保存当前参数到指定模式
uint8 key_scan(void);                               // ADC按键扫描
void menu(void);                                    // 主菜单显示与处理
void value_add_or_sub(uint32 page, uint8 arrow, uint8 mode); // PID参数加减
void key_modified(uint8 key);                       // 参数修改模式按键处理
void key_action(uint8 key);                         // 普通浏览模式按键处理

#endif
