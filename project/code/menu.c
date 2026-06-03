// ============================================================
// 文件名: menu.c
// 功能说明: 菜单处理函数（极速操作版）
// 智能车电磁循迹系统主循环，包含系统初始化、PID参数设定、
// 定时中断配置以及主循环空转。
// 
// ==================== 操作方法 ====================
// 
// 【按键定义】
//  UP/DOWN    : 上下移动光标
//  OK         : 进入/确认（主页上按OK直接加载模式，无需跳转）
//  BACK       : 返回上级页面
//  LEFT/RIGHT : 参数加/减（仅在PID修改模式下有效，LEFT增加、RIGHT减少）
//  RST        : 清屏刷新（保持当前页面和光标，仅复位屏幕显示）
//  TWO        : 全局快捷键，一键从备份重新加载PID参数（第二路ADC，约1262）
// 
// 【主页菜单】（上电默认进入）
//  1. Inductance  : 查看四路ADC电感实时数据
//  2. PID         : 进入PID参数调参页（方向环+速度环共6个参数）
//  3. Settings    : 调整基础速度和负压电机占空比
// 
// 【PID参数修改】（点击模式后直接进入）
//  页面同时显示方向环和速度环共6个参数：
//    KP_x / KI_x / KD_x / KP_v / KI_v / KD_v
//  1. UP/DOWN 切换参数项（共6项）
//  2. 按 OK 进入修改倍率：
//      - 第1次按OK：进入 *1   倍率（LEFT/RIGHT ±1.0）
//      - 第2次按OK：进入 *10  倍率（LEFT/RIGHT ±10.0）
//      - 第3次按OK：进入 *0.1  倍率（LEFT/RIGHT ±0.1）
//      - 第4次按OK：回到 *1   倍率（循环）
//  3. 在"选择参数"状态按 BACK 返回主页
//  4. 在任意倍率状态按 BACK 返回上一级倍率，直到退出修改模式
// 
// 【Settings页面】
//  1. Base Speed   : 基础目标速度（LEFT/RIGHT ±10）
//  2. Fan Duty     : 负压电机占空比（LEFT/RIGHT ±10）
//  3. 按 BACK 返回主页
// 
// 【返回逻辑】
//  所有页面按 BACK 返回上级；主页按 BACK 停留在主页
//  按 RST 仅清屏刷新，不跳转页面
//  按 TWO 任意时刻可从备份重新加载PID参数
// ============================================================
#include "config.h"


// ==================== 菜单状态变量 ====================
static uint16 page = PAGE_HOME;     // 当前页面编号
static uint8 arrow = 1;             // 当前光标位置（从1开始计数）

// 显示状态（用于减少不必要的刷新与闪烁）
static uint16 last_displayed_page = 0xFFFF;
static uint8  last_displayed_arrow = 0xFF;

// ==================== 静态函数声明 ====================
static uint8  get_item_count(uint16 page_id);
static uint16 get_parent_page(uint16 page_id);
static void   move_arrow(uint8 key);
static void   enter_page(uint16 new_page);
static void   go_back(void);
static void   menu_draw_content(void);
static void   menu_draw_cursor(void);

// ==================== 页面工具函数 ====================

// 获取指定页面的菜单项数量
static uint8 get_item_count(uint16 page_id)
{
    switch(page_id)
    {
        case PAGE_HOME:         return 3;
        case PAGE_SETTINGS:     return 2;
        case PAGE_PID_ALL:      return 6;
        default:                return 1;   // ADC页等无菜单项
    }
}

// 获取指定页面的父页面（用于返回键导航）
static uint16 get_parent_page(uint16 page_id)
{
    switch(page_id)
    {
        case PAGE_HOME:         return PAGE_HOME;
        case PAGE_ADC:
        case PAGE_SETTINGS:     return PAGE_HOME;
        case PAGE_PID_ALL:      return PAGE_HOME;
        default:                return PAGE_HOME;
    }
}

// 统一光标移动
static void move_arrow(uint8 key)
{
    uint8 count = get_item_count(page);
    if(key == UP)
    {
        arrow = (arrow <= 1) ? count : (arrow - 1);
    }
    else if(key == DOWN)
    {
        arrow = (arrow >= count) ? 1 : (arrow + 1);
    }
}

// 进入指定页面（自动重置光标）
static void enter_page(uint16 new_page)
{
    page = new_page;
    arrow = 1;
}

// 返回上级页面（修改模式会自动退出）
static void go_back(void)
{
    enter_page(get_parent_page(page));
}


// ==================== 按键扫描 ====================

// ADC按键扫描函数
// 通过两路ADC通道读取按键分压值，判断当前按下的按键
// 返回值: UP/DOWN/OK/LEFT/RIGHT/RST/BACK 或 0（无按键）
uint8 key_scan(void)
{
    uint8 key_status = 0;
    uint16 key_adc1 = 0;
    uint16 key_adc2 = 0;

    // 采集两路按键ADC，各进行3次均值滤波
    key_adc1 = adc_mean_filter_convert(KEY_CHANNEL1, 3);
    key_adc2 = adc_mean_filter_convert(KEY_CHANNEL2, 3);

    // 第一路ADC判断方向键（上/下/确定/左/右）
    switch(key_adc1 / 100)
    {
        case 5 : key_status = UP;      break;   // 约542
        case 12: key_status = DOWN;    break;   // 约1262
        case 18: key_status = OK;      break;   // 约1840
        case 24: key_status = LEFT;    break;   // 约2467
        case 30: key_status = RIGHT;   break;   // 约3082
        default:                       break;
    }

    // 第二路ADC判断功能键（复位/返回/扩展快捷键）
    // 第二路与第一路使用相同的电阻分压网络，故分压值一致
    switch(key_adc2 / 100)
    {
        case 5 : key_status = RST;     break;   // 约542
        case 12: key_status = TWO;     break;   // 约1262  一键加载平稳模式
        case 18: key_status = THREE;   break;   // 约1840  一键加载普通模式
        case 24: key_status = FOUR;    break;   // 约2467  一键加载快速模式
        case 30: key_status = BACK;    break;   // 约3082
        default:                       break;
    }
    return key_status;
}

void key_init(void)
{
	adc_init(KEY_CHANNEL1, ADC_12BIT);
	adc_init(KEY_CHANNEL2, ADC_12BIT);
	gpio_init(IO_P22, GPIO, 1, GPIO_NO_PULL);
}


// ==================== 按键动作处理（普通浏览模式） ====================

// 普通浏览模式按键处理
// 根据当前页面和按键执行页面跳转或状态切换
void key_action(uint8 key)
{
    switch(key)
    {
    case UP:
    case DOWN:
        move_arrow(key);
        break;

    case OK:
        switch(page)
        {
        case PAGE_HOME:
            if(arrow == 1)      enter_page(PAGE_ADC);
            else if(arrow == 2) { enter_page(PAGE_PID_ALL); }
            else if(arrow == 3) { enter_page(PAGE_SETTINGS); }
            break;

        }
        break;

    case BACK:
        go_back();
        break;
    }
}

// ==================== 显示绘制函数 ====================

// 绘制当前页面的静态/动态内容
static void menu_draw_content(void)
{
    switch(page)
    {
    case PAGE_HOME:
        ips114_show_string(2, 16 * 0, "Inductance");
        ips114_show_string(2, 16 * 1, "PID");
        ips114_show_string(2, 16 * 2, "Settings");
        break;

    case PAGE_SETTINGS:
        ips114_show_string(2, 0,  "Speed:");    ips114_show_uint16(90, 0,  (uint16)base_speed);
        ips114_show_string(2, 16, "Fan:");      ips114_show_uint16(90, 16, (uint16)fan_duty);
        break;

    case PAGE_ADC:
        ips114_show_string(2, 0,  "ADC1:"); ips114_show_uint16(60, 0,  (uint16)adc_filted[0]);
        ips114_show_string(2, 16, "ADC2:"); ips114_show_uint16(60, 16, (uint16)adc_filted[1]);
        ips114_show_string(2, 32, "ADC3:"); ips114_show_uint16(60, 32, (uint16)adc_filted[2]);
        ips114_show_string(2, 48, "ADC4:"); ips114_show_uint16(60, 48, (uint16)adc_filted[3]);
        break;

    case PAGE_PID_ALL:
        ips114_show_string(2, 0,  "KP_x"); ips114_show_float(60, 0,  KP_x, 1, 2);
        ips114_show_string(2, 16, "KI_x"); ips114_show_float(60, 16, KI_x, 1, 2);
        ips114_show_string(2, 32, "KD_x"); ips114_show_float(60, 32, KD_x, 1, 2);
        ips114_show_string(2, 48, "KP_v"); ips114_show_float(60, 48, KP_v, 1, 2);
        ips114_show_string(2, 64, "KI_v"); ips114_show_float(60, 64, KI_v, 1, 2);
        ips114_show_string(2, 80, "KD_v"); ips114_show_float(60, 80, KD_v, 1, 2);
        break;
    }
}

// 绘制光标指示器
static void menu_draw_cursor(void)
{
    // 浏览模式：仅在光标位置变化时更新，减少闪烁
    if(arrow != last_displayed_arrow || page != last_displayed_page)
    {
        if(last_displayed_arrow > 0 && last_displayed_arrow <= 6)
        {
            ips114_show_string(0, 16 * (last_displayed_arrow - 1), "  ");
        }
        ips114_show_string(0, 16 * (arrow - 1), ">>");
        last_displayed_arrow = arrow;
    }
}

// ==================== 主菜单函数 ====================

// 主菜单显示与处理函数
// 应在主循环中周期性调用，完成按键响应和屏幕刷新
void menu(void)
{
    static uint8 last_key = 0;  // 上一次按键值，用于边沿检测消抖
    uint8 key = key_scan();

    // 按键释放时清零记录
    if(key == 0)
    {
        last_key = 0;
    }

    // RST键处理：仅清屏刷新，保持当前页面和光标状态
    if(key == RST && key != last_key)
    {
        ips114_clear(RGB565_WHITE);
        last_displayed_page = 0xFFFF;   // 强制下次重绘
        last_displayed_arrow = 0xFF;
        last_key = key;
        return;
    }

    // 按键按下沿检测：有按键且与上次不同
    if(key != last_key && key != 0)
    {
		key_action(key);
        last_key = key;
    }

    // 页面切换检测：仅在切换时执行清屏，避免每帧闪烁
    if(page != last_displayed_page)
    {
        ips114_clear(RGB565_WHITE);
        last_displayed_page = page;
        last_displayed_arrow = 0xFF;    // 强制重绘光标
    }

    // 绘制页面内容
    menu_draw_content();

    // 绘制光标指示器
    menu_draw_cursor();
}
