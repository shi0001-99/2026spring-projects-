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

