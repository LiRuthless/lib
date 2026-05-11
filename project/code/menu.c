// ============================================================
// 文件名: menu.c
// 功能说明: 菜单处理函数（优化版）
// 智能车电磁循迹系统主循环，包含系统初始化、PID参数设定、
// 定时中断配置以及主循环空转。
// 
// ==================== 操作方法 ====================
// 
// 【按键定义】
//  UP/DOWN    : 上下移动光标
//  OK         : 进入/确认
//  BACK       : 返回上级页面
//  LEFT/RIGHT : 参数加/减（仅在PID修改模式下有效，LEFT增加、RIGHT减少）
//  RST        : 复位，返回主页
// 
// 【主页菜单】（上电默认进入）
//  1. Inductance  : 查看四路ADC电感实时数据
//  2. Settings    : 进入模式选择页
//  3. Start       : （预留）
//  4. Run         : （预留）
// 
// 【模式选择页】
//  1. Load Steady   : 加载平稳模式PID参数
//  2. Load Ordinary : 加载普通模式PID参数
//  3. Load Fast     : 加载快速模式PID参数
//  4. PID Params    : 进入PID参数修改页
// 
// 【PID参数修改】
//  1. 进入"PID Params"后选择 Direction PID（方向环）或 Speed PID（速度环）
//  2. 按 OK 进入修改模式：
//      - 第1次按OK：进入 *1   倍率（LEFT/RIGHT ±1.0）
//      - 第2次按OK：进入 *10  倍率（LEFT/RIGHT ±10.0）
//      - 第3次按OK：进入 *0.1 倍率（LEFT/RIGHT ±0.1）
//      - 第4次按OK：回到 *1   倍率（循环）
//  3. 在"选择参数"状态（无倍率显示）按 BACK 返回PID类型页
//  4. 在任意倍率状态按 BACK 返回上一级倍率，直到退出修改模式
//  5. UP/DOWN 在所有修改状态下均可切换参数项（Kp / Ki / Kd）
// 
// 【返回逻辑】
//  所有页面按 BACK 返回上级；主页按 BACK 停留在主页
//  按 RST 任意时刻直接复位到主页
// 
// ============================================================
#include "config.h"

// ==================== PID参数结构体 ====================
typedef struct
{
    float kp_x, ki_x, kd_x;
    float kp_v, ki_v, kd_v;
} PidParams;

// 三种模式参数存储（平稳/普通/快速）
static PidParams mode_params[3] = {0};

// 参数指针数组（用于修改页面快速访问，避免冗长的switch-case）
static float* const pid_dir_vars[3]  = {&KP_x, &KI_x, &KD_x};
static float* const pid_speed_vars[3]= {&KP_v, &KI_v, &KD_v};

// ==================== 菜单状态变量 ====================
static uint16 page = PAGE_HOME;     // 当前页面编号
static uint8 arrow = 1;             // 当前光标位置（从1开始计数）
static uint8 modified_order = 0;    // 0=普通浏览模式, 1=参数修改模式
static uint8 modified_mode = 1;     // 修改状态机：1=选择参数, 2=*1倍率, 3=*10倍率, 4=*0.1倍率
static uint8 current_mode = MODE_STEADY;  // 当前加载的模式

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
        case PAGE_HOME:         return 4;
        case PAGE_MODE_SELECT:  return 4;
        case PAGE_PID_TYPE:     return 2;
        case PAGE_PID_DIR:
        case PAGE_PID_SPEED:    return 3;
        default:                return 1;   // ADC页、确认页等无菜单项
    }
}

// 获取指定页面的父页面（用于返回键导航）
static uint16 get_parent_page(uint16 page_id)
{
    switch(page_id)
    {
        case PAGE_HOME:         return PAGE_HOME;
        case PAGE_ADC:
        case PAGE_MODE_SELECT:  return PAGE_HOME;
        case PAGE_PID_TYPE:
        case PAGE_MODE_STEADY:
        case PAGE_MODE_ORDINARY:
        case PAGE_MODE_FAST:    return PAGE_MODE_SELECT;
        case PAGE_PID_DIR:
        case PAGE_PID_SPEED:    return PAGE_PID_TYPE;
        default:                return PAGE_HOME;
    }
}

// 统一光标移动（修复原代码key_modified中UP/DOWN同向的bug）
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
    if(page == PAGE_PID_DIR || page == PAGE_PID_SPEED)
    {
        modified_order = 0;
        modified_mode = 1;
    }
    enter_page(get_parent_page(page));
}

// ==================== 模式切换与参数保存 ====================

// 根据模式索引加载对应的PID参数到活跃变量
void switch_mode(uint8 mode)
{
    if(mode >= 3) return;
    KP_x = mode_params[mode].kp_x;
    KI_x = mode_params[mode].ki_x;
    KD_x = mode_params[mode].kd_x;
    KP_v = mode_params[mode].kp_v;
    KI_v = mode_params[mode].ki_v;
    KD_v = mode_params[mode].kd_v;
    current_mode = mode;
}

// 保存当前活跃参数到指定模式存储区
void save_current_param(uint8 mode)
{
    if(mode >= 3) return;
    mode_params[mode].kp_x = KP_x;
    mode_params[mode].ki_x = KI_x;
    mode_params[mode].kd_x = KD_x;
    mode_params[mode].kp_v = KP_v;
    mode_params[mode].ki_v = KI_v;
    mode_params[mode].kd_v = KD_v;
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

    // 第二路ADC判断功能键（复位/返回）
    switch(key_adc2 / 100)
    {
        case 5 : key_status = RST;     break;   // 约542
        case 30: key_status = BACK;    break;   // 约3082
        default:                       break;
    }
    return key_status;
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
            else if(arrow == 2) enter_page(PAGE_MODE_SELECT);
            // arrow 3/4 (Start/Run) 在原代码中无OK响应，保持预留
            break;

        case PAGE_MODE_SELECT:
            if(arrow == 1)      { switch_mode(MODE_STEADY);   enter_page(PAGE_MODE_STEADY); }
            else if(arrow == 2) { switch_mode(MODE_ORDINARY); enter_page(PAGE_MODE_ORDINARY); }
            else if(arrow == 3) { switch_mode(MODE_FAST);     enter_page(PAGE_MODE_FAST); }
            else if(arrow == 4) { enter_page(PAGE_PID_TYPE); }
            break;

        case PAGE_PID_TYPE:
            if(arrow == 1)      { enter_page(PAGE_PID_DIR);   modified_order = 1; modified_mode = 1; }
            else if(arrow == 2) { enter_page(PAGE_PID_SPEED); modified_order = 1; modified_mode = 1; }
            break;
        }
        break;

    case BACK:
        go_back();
        break;
    }
}

// ==================== PID参数修改模式按键处理 ====================

// PID参数修改模式按键处理
// 支持四种修改倍率：选择模式 -> *1 -> *10 -> *0.1 -> 循环
void key_modified(uint8 key)
{
    switch(key)
    {
    case UP:
    case DOWN:
        move_arrow(key);
        break;

    case OK:
        // 状态机递进：1->2, 2->3, 3->4, 4->2(循环)
        if(modified_mode == 4) modified_mode = 2;
        else modified_mode++;
        break;

    case BACK:
        // 状态机回退，回到选择模式则退出修改
        if(modified_mode == 1)
        {
            modified_order = 0;
            modified_mode = 1;
            go_back();
        }
        else
        {
            modified_mode--;
        }
        break;

    case LEFT:
        // 倍率状态：2=*1, 3=*10, 4=*0.1
        // 映射到value_add_or_sub的mode：1=-1, 2=+1, 3=-10, 4=+10, 5=-0.1, 6=+0.1
        if(modified_mode >= 2)
        {
            value_add_or_sub(page, arrow, (modified_mode - 2) * 2 + 1);
        }
        break;

    case RIGHT:
        if(modified_mode >= 2)
        {
            value_add_or_sub(page, arrow, (modified_mode - 2) * 2 + 2);
        }
        break;
    }
}

// ==================== PID参数值加减 ====================

// PID参数值加减函数
// page: 当前页面(PAGE_PID_DIR=方向PID, PAGE_PID_SPEED=速度PID)
// arrow: 当前选中的参数行(1=Kp, 2=Ki, 3=Kd)
// mode: 加减模式(1=-1, 2=+1, 3=-10, 4=+10, 5=-0.1, 6=+0.1)
void value_add_or_sub(uint32 page, uint8 arrow, uint8 mode)
{
    // 步长映射表（index对应mode值，LEFT增加/RIGHT减少，保持原操作习惯）
    static const float delta_map[7] = {0.0f, 1.0f, -1.0f, 10.0f, -10.0f, 0.1f, -0.1f};
    float delta;

    if(mode < 1 || mode > 6) return;
    if(arrow < 1 || arrow > 3) return;

    delta = delta_map[mode] * 0.10f;

    // 通过指针数组直接修改对应参数，避免冗长的switch-case
    if(page == PAGE_PID_DIR)
    {
        *pid_dir_vars[arrow - 1] += delta;
    }
    else if(page == PAGE_PID_SPEED)
    {
        *pid_speed_vars[arrow - 1] += delta;
    }

    // 活跃参数直接供控制环使用，保存到当前模式
    save_current_param(current_mode);
}

// ==================== 显示绘制函数 ====================

// 绘制当前页面的静态/动态内容
static void menu_draw_content(void)
{
    switch(page)
    {
    case PAGE_HOME:
        ips114_show_string(2, 16 * 0, "Inductance");
        ips114_show_string(2, 16 * 1, "Settings");
        ips114_show_string(2, 16 * 2, "Start");
        ips114_show_string(2, 16 * 3, "Run");
        break;

    case PAGE_ADC:
        ips114_show_string(2, 0,  "ADC1:"); ips114_show_uint16(60, 0,  (uint16)adc_filted[0]);
        ips114_show_string(2, 16, "ADC2:"); ips114_show_uint16(60, 16, (uint16)adc_filted[1]);
        ips114_show_string(2, 32, "ADC3:"); ips114_show_uint16(60, 32, (uint16)adc_filted[2]);
        ips114_show_string(2, 48, "ADC4:"); ips114_show_uint16(60, 48, (uint16)adc_filted[3]);
        break;

    case PAGE_MODE_SELECT:
        ips114_show_string(20, 16 * 0, "Load Steady");
        ips114_show_string(20, 16 * 1, "Load Ordinary");
        ips114_show_string(20, 16 * 2, "Load Fast");
        ips114_show_string(20, 16 * 3, "PID Params");
        break;

    case PAGE_MODE_STEADY:
    case PAGE_MODE_ORDINARY:
    case PAGE_MODE_FAST:
        // 模式加载确认页（空白，原设计如此）
        break;

    case PAGE_PID_TYPE:
        ips114_show_string(20, 16 * 0, "Direction PID");
        ips114_show_string(20, 16 * 1, "Speed PID");
        break;

    case PAGE_PID_DIR:
        ips114_show_string(2, 0,  "KP_x"); ips114_show_float(60, 0,  KP_x, 1, 2);
        ips114_show_string(2, 16, "KI_x"); ips114_show_float(60, 16, KI_x, 1, 2);
        ips114_show_string(2, 32, "KD_x"); ips114_show_float(60, 32, KD_x, 1, 2);
        break;

    case PAGE_PID_SPEED:
        ips114_show_string(2, 0,  "KP_v"); ips114_show_float(60, 0,  KP_v, 1, 2);
        ips114_show_string(2, 16, "KI_v"); ips114_show_float(60, 16, KI_v, 1, 2);
        ips114_show_string(2, 32, "KD_v"); ips114_show_float(60, 32, KD_v, 1, 2);
        break;
    }
}

// 绘制光标或修改模式指示器
static void menu_draw_cursor(void)
{
    uint8 i;
    uint8 count = get_item_count(page);

    if(modified_order == 0)
    {
        // 浏览模式：仅在光标位置变化时更新，减少闪烁
        if(arrow != last_displayed_arrow || page != last_displayed_page)
        {
            if(last_displayed_arrow > 0 && last_displayed_arrow <= 4)
            {
                ips114_show_string(0, 16 * (last_displayed_arrow - 1), "  ");
            }
            ips114_show_string(0, 16 * (arrow - 1), ">>");
            last_displayed_arrow = arrow;
        }
    }
    else
    {
        // 修改模式：先清除所有行标记，再绘制当前行
        for(i = 0; i < count; i++)
        {
            ips114_show_string(0, 16 * i, " ");
        }

        // 显示当前修改倍率
        if(modified_mode == 2)      ips114_show_string(200, 0, "*1   ");
        else if(modified_mode == 3) ips114_show_string(200, 0, "*10  ");
        else if(modified_mode == 4) ips114_show_string(200, 0, "*0.1 ");
        else                        ips114_show_string(200, 0, "     ");

        // 显示当前行标记：选择模式用">"，修改模式用"="
        if(modified_mode == 1)
        {
            ips114_show_string(0, 16 * (arrow - 1), ">");
        }
        else
        {
            ips114_show_string(0, 16 * (arrow - 1), "=");
        }

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

    // RST键（复位键）处理：短按复位屏幕和菜单状态
    if(key == RST && key != last_key)
    {
        enter_page(PAGE_HOME);
        modified_order = 0;
        modified_mode = 1;
        last_key = key;
        return;
    }

    // 按键按下沿检测：有按键且与上次不同
    if(key != last_key && key != 0)
    {
        if(modified_order == 0)
        {
            key_action(key);
        }
        else
        {
            key_modified(key);
        }
    }
    last_key = key;

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
