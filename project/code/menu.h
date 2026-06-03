#ifndef _menu_h_
#define _menu_h_

// 按键值枚举
#define OK      1       // 确定键
#define UP      2       // 上键
#define DOWN    3       // 下键
#define BACK    4       // 返回键
#define LEFT    5       // 左键（参数减）
#define RIGHT   6       // 右键（参数加）
#define RST     7       // 复位键（清屏刷新，保持当前页面）
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
#define PAGE_SETTINGS       4   // 基础速度与负压电机占空比设置
#define PAGE_PID_ALL        40  // 综合PID参数修改（方向+速度）


// 函数声明
uint8 key_scan(void);                               // ADC按键扫描
void key_init(void);
void menu(void);                                    // 主菜单显示与处理
void key_action(uint8 key);                         // 普通浏览模式按键处理

#endif
