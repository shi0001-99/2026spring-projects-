#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <windows.h> 

using namespace std;

const int MAX_TIME = 120;          // 每局时间120秒
const int MAX_PATIENCE = 40;       // 顾客耐心值
const int INVENTORY_MAX = 10;      // 库存上限
const int MAX_CUSTOMERS = 3;       // 最大顾客数（基础M=1，可升级到3）
const int BURRITO_VALUE = 20;      // 卷饼价值
const int COLA_VALUE = 10;         // 可乐价值
const int FRIES_VALUE = 5;         // 薯条价值

// 物品索引
const int ITEM_COUNT = 12;
const int BREAD = 0;       // 面饼
const int MEAT = 1;        // 肉
const int CUCUMBER = 2;    // 黄瓜
const int SAUCE = 3;       // 沙司
const int FRIES_ING = 4;   // 薯条配料
const int KETCHUP = 5;     // 番茄酱
const int WRAPPER = 6;     // 包装纸
const int FRIES_BOX = 7;   // 薯条盒
const int COLA_CUP = 8;    // 可乐杯
const int FRIES = 9;       // 炸薯条
const int COLA = 10;       // 可乐液
const int POTATO = 11;     // 土豆


string itemNames[ITEM_COUNT] = {
    "饼", "肉", "瓜", "酱", "薯配", "番",
    "纸", "盒", "杯", "薯条", "可乐液", "土豆"
};


int totalGold = 1000;         // 累计金币
int totalDays = 0;         // 进行局数
int shopLevel = 0;         // 店铺等级（0:基础，1:有金盘子，2:有自动切肉机，3:扩充店面）
int currentGold = 0;       // 本局金币
int currentTime = 0;       // 本局剩余时间
int currentDay = 1;        // 当前天数

//库存数组
int inventory[ITEM_COUNT];

// 卷饼制作状态
int burritoStage = 0;              // 0:空, 1:有饼, 2:有配料, 3:已卷, 4:已包
int burritoIngredients[5] = { 0 };   // [肉,黄瓜,沙司,薯条料,番茄酱]

// 小吃制作状态
int friesStage = 0;    // 0:空, 1:有盒, 2:已装薯条
int colaStage = 0;     // 0:空, 1:有杯, 2:已接可乐

// 顾客系统
int customerCount = 0;                     // 当前顾客数
int customerPatience[MAX_CUSTOMERS];       // 顾客耐心
int customerWantsBurrito[MAX_CUSTOMERS];   // 是否要卷饼
int customerWantsFries[MAX_CUSTOMERS];     // 是否要薯条
int customerWantsCola[MAX_CUSTOMERS];      // 是否要可乐
int customerBurritoNeed[5][MAX_CUSTOMERS]; // 卷饼需求[5种配料][顾客]

// ==================== 函数声明 ====================
void initGame();
void showMainMenu();
void showUpgradeMenu();
void showGameScreen();
void showHelp();
void updateGame();
void generateCustomer();
void processCommand(char cmd);
void restockItem(int item);
bool makeBurritoStep(int step);
bool makeFriesStep(int step);
bool makeColaStep(int step);
bool serveCustomer(int custId);
void cutMeat();
void fryFries();

// 初始化游戏
void initGame() {
    // 初始化库存
    for (int i = 0; i < ITEM_COUNT; i++) {
        inventory[i] = INVENTORY_MAX;
    }

    // 初始化制作状态
    burritoStage = 0;
    for (int i = 0; i < 5; i++) burritoIngredients[i] = 0;
    friesStage = 0;
    colaStage = 0;

    // 初始化顾客
    customerCount = 0;
    for (int i = 0; i < MAX_CUSTOMERS; i++) {
        customerPatience[i] = 0;
        customerWantsBurrito[i] = 0;
        customerWantsFries[i] = 0;
        customerWantsCola[i] = 0;
        for (int j = 0; j < 5; j++) customerBurritoNeed[j][i] = 0;
    }

    currentGold = 0;
    currentTime = MAX_TIME;
}

// 显示主菜单
void showMainMenu() {
    system("cls");
    cout << "========================================\n";
    cout << "          沙威玛传奇（控制台版）\n";
    cout << "========================================\n\n";

    cout << "累计金币： "<< totalGold << "\n";
    cout << "进行局数： "<< totalDays << "\n";
    cout << "店铺升级： ";

    if (shopLevel == 0) cout << "无\n";
    else {
        if (shopLevel & 1) cout << "金盘子 ";
        if (shopLevel & 2) cout << "自动切肉机 ";
        if (shopLevel & 4) cout << "扩充店面";
        cout << "\n";
    }

    cout << "\n----------------------------------------\n";
    cout << "1. 开始新的一天\n";
    cout << "2. 购买升级 - 金盘子（售价 100金币，卷饼价值增加）\n";
    cout << "3. 购买升级 - 自动切肉机（售价 150金币，自动补肉）\n";
    cout << "4. 购买升级 - 扩充店面（售价 200金币，可容纳更多顾客）\n";
    cout << "5. 退出游戏\n";
    cout << "----------------------------------------\n";
    cout << "请输入选项：";
}

// 显示升级菜单
void showUpgradeMenu() {
    int choice;
    system("cls");
    cout << "========================================\n";
    cout << "              店铺升级\n";
    cout << "========================================\n\n";
    cout << "当前金币：" << totalGold << "\n\n";

    cout << "1. 金盘子 - 100金币\n";
    cout << "   卷饼价值从10金币增加到20金币\n";
    if (shopLevel & 1) cout << "   [已购买]\n\n";

    cout << "2. 自动切肉机 - 150金币\n";
    cout << "   肉库存用完自动补满\n";
    if (shopLevel & 2) cout << "   [已购买]\n\n";

    cout << "3. 扩充店面 - 200金币\n";
    cout << "   可同时服务更多顾客\n";
    if (shopLevel & 4) cout << "   [已购买]\n\n";

    cout << "4. 返回主菜单\n\n";
    cout << "请选择：";
    cin >> choice;

    switch (choice) {
    case 1:
        if (!(shopLevel & 1) && totalGold >= 100) {
            totalGold -= 100;
            shopLevel |= 1;
            cout << "购买金盘子成功！\n";
        }
        else if (shopLevel & 1) {
            cout << "已购买过此升级！\n";
        }
        else {
            cout << "金币不足！\n";
        }
        break;
    case 2:
        if (!(shopLevel & 2) && totalGold >= 150) {
            totalGold -= 150;
            shopLevel |= 2;
            cout << "购买自动切肉机成功！\n";
        }
        else if (shopLevel & 2) {
            cout << "已购买过此升级！\n";
        }
        else {
            cout << "金币不足！\n";
        }
        break;
    case 3:
        if (!(shopLevel & 4) && totalGold >= 200) {
            totalGold -= 200;
            shopLevel |= 4;
            cout << "扩充店面成功！\n";
        }
        else if (shopLevel & 4) {
            cout << "已购买过此升级！\n";
        }
        else {
            cout << "金币不足！\n";
        }
        break;
    case 4:
        return;
    }

    cout << "按回车键继续...";
    cin.ignore();
    cin.get();
}