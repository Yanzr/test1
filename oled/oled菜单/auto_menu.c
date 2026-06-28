#include "auto_menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int8_t scroll_offset = 0;  // 当前显示菜单项的起始位置

// 实现 strdup 函数，用于复制字符串
char *strdup(const char *s)
{
    if (s == NULL)
        return NULL;

    size_t len = strlen(s) + 1; // 加 1 是为了包括末尾的 '\0'
    char *copy = (char *)malloc(len);

    if (copy != NULL)
    {
        memcpy(copy, s, len); // 复制字符串内容
    }

    return copy;
}


// 定义全局变量
static MenuItem *current_menu = NULL;                // 当前菜单指针
static MenuItem *parent_menu_stack[MENU_STACK_SIZE]; // 父菜单堆栈
static int parent_menu_stack_index = -1;             // 堆栈索引
static int current_level = 0;                        // 当前菜单层级

// 创建一个菜单项
MenuItem *create_menu_item(const char *name, MenuItemType type, void (*action)(void), MenuItem *submenu)
{
    MenuItem *item = (MenuItem *)malloc(sizeof(MenuItem));
    if (!item)
    {
        return NULL; // 内存分配失败时返回 NULL
    }
    item->name = strdup(name); // 复制菜单项名称
    item->type = type;
    if (type == MENU_ITEM_ACTION)
    {
        item->menu_item.action = action; // 为动作类型设置回调函数
    }
    else if (type == MENU_ITEM_SUBMENU)
    {
        item->menu_item.submenu = submenu; // 为子菜单类型设置子菜单指针
    }
    else if (type == MENU_ITEM_BOOLEAN)
    {
        item->menu_item.boolean_item.value = NULL;         // 初始设置为空，需在外部设置
        item->menu_item.boolean_item.toggle_action = NULL; // 初始设置为空，需在外部设置
    }
    else if (type == MENU_ITEM_NUMERIC)
    {
        item->menu_item.numeric_item.value = NULL;         // 初始设置为空，需在外部设置
        item->menu_item.numeric_item.min = 0;              // 初始最小值为0
        item->menu_item.numeric_item.max = 100;            // 初始最大值为100
        item->menu_item.numeric_item.adjust_action = NULL; // 初始设置为空，需在外部设置
    }
    item->next = NULL;
    item->prev = NULL;
    return item;
}

// 将菜单项添加到链表中
void add_menu_item(MenuItem **head, MenuItem *item)
{
    if (*head == NULL)
    {
        *head = item; // 如果链表为空，将新项设为头节点
    }
    else
    {
        MenuItem *temp = *head;
        while (temp->next != NULL)
        {
            temp = temp->next; // 遍历到链表末尾
        }
        temp->next = item; // 将新项添加到链表末尾
        item->prev = temp; // 设置前一项的指针
    }
}

// 删除菜单项
void remove_menu_item(MenuItem **head, const char *name)
{
    if (!head || !*head || !name)
        return;

    MenuItem *temp = *head;
    while (temp != NULL)
    {
        if (strcmp(temp->name, name) == 0)
        {
            if (temp->prev)
                temp->prev->next = temp->next;
            if (temp->next)
                temp->next->prev = temp->prev;

            if (temp == *head)
                *head = temp->next;

            free(temp->name);
            free(temp);
            return;
        }
        temp = temp->next;
    }
}

// 更新菜单项
void update_menu_item(MenuItem *item, const char *new_name, void (*new_action)(void))
{
    if (!item || !new_name)
        return;

    free(item->name);
    item->name = strdup(new_name);
    if (item->type == MENU_ITEM_ACTION)
    {
        item->menu_item.action = new_action;
    }
}

// 释放菜单结构的内存
void free_menu(MenuItem *menu)
{
    MenuItem *temp;
    while (menu)
    {
        temp = menu;
        menu = menu->next;
        free(temp->name); // 释放菜单项名称的内存
        free(temp);       // 释放菜单项的内存
    }
}

// 初始化菜单
void auto_menu_init(MenuItem *root)
{
    current_menu = root;          // 设置当前菜单为根菜单
    parent_menu_stack_index = -1; // 重置父菜单堆栈索引
    current_level = 0;            // 重置菜单层级
    auto_menu_display();          // 初始化后立即显示菜单
}

uint8_t para_set_flag = 0; // 数值设置模式标志


uint8_t get_menu_item_count(void)
{
    uint8_t count = 0;
    MenuItem *temp = current_menu;
    
    // 遍历当前菜单项链表，统计项数
    while (temp != NULL)
    {
        count++;
        temp = temp->next;
    }
    
    return count;  // 返回菜单项数量
}

// 处理按键操作
void auto_menu_handle_key(uint8_t key)
{
    // 如果当前菜单项是数值类型，使用专用的设置逻辑
    if (current_menu && current_menu->type == MENU_ITEM_NUMERIC && para_set_flag == 1)
    {
        switch (key)
        {
        case 1: // 增加数值
            if (current_menu->menu_item.numeric_item.adjust_action)
            {
                current_menu->menu_item.numeric_item.adjust_action(1); // 调增1
            }
            break;
        case 2: // 减少数值
            if (current_menu->menu_item.numeric_item.adjust_action)
            {
                current_menu->menu_item.numeric_item.adjust_action(-1); // 调减1
            }
            break;
        case 4: // 退出数值设置
            para_set_flag = 0; // 退出数值设置模式
            break;
        }
    }
    else
    {
        // 处理非设置模式下的菜单导航逻辑
        switch (key)
        {
        case 1: // 向上移动
            if (current_menu && current_menu->prev != NULL) // 向上跳到上一个菜单项
            {
                current_menu = current_menu->prev;
				
                // 如果当前选中项位于显示范围之外，更新滚动偏移
                if (scroll_offset > 0)
                {
                    scroll_offset--;
                }
            }
            break;

        case 2: // 向下移动
            if (current_menu && current_menu->next != NULL) // 向下跳到下一个菜单项
            {
				
                current_menu = current_menu->next;

                // 如果当前选中项位于显示范围之外，更新滚动偏移
                if (scroll_offset + MAX_LINES < get_menu_item_count())
                {
                    scroll_offset++;
                }
            }
            break;

        case 3: // 选择或操作当前项
            if (current_menu)
            {
                switch (current_menu->type)
                {
                case MENU_ITEM_ACTION:
                    if (current_menu->menu_item.action)
                    {
                        current_menu->menu_item.action(); // 执行动作
                    }
                    break;

                case MENU_ITEM_SUBMENU:
                    if (parent_menu_stack_index < MENU_STACK_SIZE - 1)
                    {
                        parent_menu_stack[++parent_menu_stack_index] = current_menu; // 压入堆栈
                        current_menu = current_menu->menu_item.submenu;              // 进入子菜单
                        current_level++;                                             // 增加层级
                    }
                    break;

                case MENU_ITEM_BOOLEAN:
                    if (current_menu->menu_item.boolean_item.toggle_action)
                    {
                        current_menu->menu_item.boolean_item.toggle_action(); // 切换布尔值
                    }
                    break;

                case MENU_ITEM_NUMERIC:
                    para_set_flag = 1; // 进入数值设置模式
                    break;
                }
            }
            break;

        case 4: // 返回上一级菜单
            if (parent_menu_stack_index >= 0)
            {
                current_menu = parent_menu_stack[parent_menu_stack_index--];
                current_level--;
                scroll_offset = 0; // 返回上一级时重置滚动偏移
            }
            break;
        }
    }
		//printf("scroll_offset,%d\n",scroll_offset);
	//	printf("all:%d\n",get_menu_item_count());
		auto_menu_display(); // 刷新菜单显示
}

void auto_menu_display(void)
{
    if (current_menu == NULL)
    {
        menu_init(); // 如果当前菜单为空，重新初始化菜单
        return;
    }

    OLED_operate_gram(PEN_CLEAR);  // 清空屏幕

    MenuItem *temp = current_menu;
    uint8_t line = 0;

    // 显示当前层级
    char level_str[20];
    snprintf(level_str, sizeof(level_str), para_set_flag ? "Level: %d (Set)" : "Level: %d", current_level);
    OLED_ShowString(0, line * 12, (uint8_t *)level_str, 1); // 显示当前层级
    line++; // 增加一行

    // 获取菜单项的总数
    uint8_t menu_item_count = get_menu_item_count();

    // 如果菜单项总数小于显示行数，显示所有菜单项
    if (menu_item_count <= MAX_LINES)
    {
        scroll_offset = 0; // 如果菜单项较少，确保从第一项开始显示
    }
    else
    {
        // 否则，判断滚动范围
        if (scroll_offset > menu_item_count - MAX_LINES)
        {
            scroll_offset = menu_item_count - MAX_LINES;
        }

        if (scroll_offset < 0)
        {
            scroll_offset = 0;
        }
    }

    // 显示从 scroll_offset 开始的菜单项
    temp = current_menu;
    for (uint8_t i = 0; i < scroll_offset; i++) // 跳过前面的菜单项
    {
        if (temp != NULL)
        {
            temp = temp->next;
        }
    }

    uint8_t line_counter = 0;
    while (temp != NULL && line_counter < MAX_LINES)
    {
        if (temp->name == NULL || temp->name[0] == '\0')
        {
            break;
        }

        char display_text[50];
        snprintf(display_text, sizeof(display_text), "%s", temp->name);

        // 根据菜单项类型显示附加信息
        if (temp->type == MENU_ITEM_BOOLEAN && temp->menu_item.boolean_item.value != NULL)
        {
            strcat(display_text, (*(temp->menu_item.boolean_item.value) ? " ON" : " OFF"));
        }
        else if (temp->type == MENU_ITEM_NUMERIC && temp->menu_item.numeric_item.value != NULL)
        {
            char value_str[10];
            snprintf(value_str, sizeof(value_str), " %.2f", *(temp->menu_item.numeric_item.value));
			printf(" Speed: %.2f\n", motor1Speed);
            strcat(display_text, value_str);
        }

        // 显示菜单项，判断是否为当前选中的菜单项
        OLED_ShowString(0, (line_counter + line) * 12, (uint8_t *)display_text, 1); // 正常显示
        if (temp == current_menu) // 如果是选中的菜单项
        {
            OLED_DrawRectangle(0, (line_counter + line) * 12, 127, (line_counter + line) * 12 + 11, 1); // 画边框
        }

        temp = temp->next;
        line_counter++;
    }

    OLED_refresh_gram(); // 刷新显示
}


#include <stdio.h>

uint8_t motor1flag = 0; // 全局布尔值变量
int para_val = 50;    // 全局数值变量



// 布尔开关的回调函数
void motor1mode(void)
{
    motor1flag = !motor1flag; // 切换布尔值
    printf("motor1 mode: %d\n", motor1flag);
	if(motor1flag)
	Motor_SetPWM(1,adc_value[0]*30);
	else
	Motor_SetPWM(1,0);	
}

// 数值调整的回调函数
void adjust_value(int adjustment)
{
//    para_val += adjustment; // 调整数值
//    if (para_val < 0)
//        para_val = 0;
//    if (para_val > 100)
//        para_val = 100;
//    printf("Value adjusted: %d\n", para_val);
}

void menu_init(void)
{
    // 初始化菜单结构，配置菜单项

    MenuItem *root_menu = create_menu_item("Main Menu", MENU_ITEM_SUBMENU, NULL, NULL);
	
    MenuItem *motor1menu = create_menu_item("motor1", MENU_ITEM_SUBMENU, NULL, NULL);
	
	MenuItem *motormode_item = create_menu_item("motormode", MENU_ITEM_BOOLEAN, NULL, NULL);
	motormode_item->menu_item.boolean_item.value = &motor1flag;
    motormode_item->menu_item.boolean_item.toggle_action = motor1mode;
	
	
//    MenuItem *action_item1 = create_menu_item("Action 1", MENU_ITEM_ACTION, action_1, NULL);
//    MenuItem *boolean_item = create_menu_item("Toggle Bool", MENU_ITEM_BOOLEAN, NULL, NULL);
//    boolean_item->menu_item.boolean_item.value = &bool_val;
//    boolean_item->menu_item.boolean_item.toggle_action = toggle_bool;

    MenuItem *motor1_p = create_menu_item("motor1_p", MENU_ITEM_NUMERIC, NULL, NULL);
    motor1_p->menu_item.numeric_item.value = &adc_value[0];
    motor1_p->menu_item.numeric_item.min = 0;
    motor1_p->menu_item.numeric_item.max = 100;
    motor1_p->menu_item.numeric_item.adjust_action = adjust_value;
	
    MenuItem *motor1_act_spe = create_menu_item("act_spe", MENU_ITEM_NUMERIC, NULL, NULL);
    motor1_act_spe->menu_item.numeric_item.value = &motor1Speed;
    motor1_act_spe->menu_item.numeric_item.min = 0;
    motor1_act_spe->menu_item.numeric_item.max = 1000;
    motor1_act_spe->menu_item.numeric_item.adjust_action = adjust_value;	
	
	
//	MenuItem *action_item2 = create_menu_item("Action 2", MENU_ITEM_ACTION, action_1, NULL);
//	MenuItem *action_item3 = create_menu_item("Action 3", MENU_ITEM_ACTION, action_1, NULL);
//	MenuItem *action_item4 = create_menu_item("Action 4", MENU_ITEM_ACTION, action_1, NULL);
//	MenuItem *action_item5 = create_menu_item("Action 5", MENU_ITEM_ACTION, action_1, NULL);

//    // 构建菜单层次结构


		
	add_menu_item(&root_menu->menu_item.submenu, motor1menu);
	add_menu_item(&motor1menu->menu_item.submenu, motormode_item);
	add_menu_item(&motor1menu->menu_item.submenu, motor1_p);
	add_menu_item(&motor1menu->menu_item.submenu, motor1_act_spe);
	


    // 初始化菜单系统
    auto_menu_init(root_menu);
	
}
