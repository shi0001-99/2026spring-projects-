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

// 显示游戏界面
void showGameScreen() {
    system("cls");

    // 状态栏
    cout << "[第 " << currentDay << " 天] | ";
    cout << "时间: " <<  currentTime << "s | ";
    cout << "金币: " << currentGold << "\n";

    int burritoVal = (shopLevel & 1) ? 30 : 20;
    cout << "卷饼" << burritoVal << "金币/个    ";
    cout << "可乐" << COLA_VALUE << "金币/杯    ";
    cout << "薯条" << FRIES_VALUE << "金币/份\n";
    cout << "----------------------------------------\n";

    // 顾客订单区
    cout << "顾客订单";
    int maxCustomers = (shopLevel & 4) ? 3 : 1;
    if (customerCount > 0) {
        cout << ":\n";
        for (int i = 0; i < customerCount && i < maxCustomers; i++) {
            cout << "顾客" << i + 1 << " (耐心:" << customerPatience[i] << "s):\n";

            if (customerWantsBurrito[i]) {
                cout << "-卷饼需求: ";
                if (customerBurritoNeed[0][i]) cout << "肉 ";
                if (customerBurritoNeed[1][i]) cout << "黄瓜 ";
                if (customerBurritoNeed[2][i]) cout << "沙司 ";
                if (customerBurritoNeed[3][i]) cout << "薯条 ";
                if (customerBurritoNeed[4][i]) cout << "番茄酱 ";
                cout << "\n";
            }

            cout << "-小吃：";
            if (customerWantsFries[i]) cout << "薯条 ";
            if (customerWantsCola[i]) cout << "可乐 ";
            cout << "\n";
            
        }
    }
    else {
        cout << ": 等待顾客...\n";
    }
    cout << "----------------------------------------\n";

    // 操作台
    cout << "【卷饼台】 ";
    switch (burritoStage) {
    case 0: cout << "空"; break;
    case 1: cout << "有饼"; break;
    case 2: cout << "有料"; break;
    case 3: cout << "已卷"; break;
    case 4: cout << "已包"; break;
    }

    cout << " | 配料: [";
    if (burritoIngredients[0]) cout << "肉";
    if (burritoIngredients[1]) cout << "瓜";
    if (burritoIngredients[2]) cout << "酱";
    if (burritoIngredients[3]) cout << "薯";
    if (burritoIngredients[4]) cout << "番";
    cout << "] | ";

    if (burritoStage < 3) cout << "未卷 | ";
    if (burritoStage < 4) cout << "未包";
    cout << "\n";

    cout << "【小吃台】 ";
    cout << "薯条:";
    switch (friesStage) {
    case 0: cout << "无"; break;
    case 1: cout << "盒"; break;
    case 2: cout << "满"; break;
    }
    cout << " | 可乐:";
    switch (colaStage) {
    case 0: cout << "无"; break;
    case 1: cout << "杯"; break;
    case 2: cout << "满"; break;
    }
    cout << "\n";
    cout << "----------------------------------------\n";

    // 库存状态区
    cout << "[主料] 饼:" << setw(2) << inventory[BREAD];
    cout << " 肉:" << setw(2) << inventory[MEAT] << "\n";
    cout << "[配料] 瓜:" << setw(2) << inventory[CUCUMBER];
    cout << " 酱:" << setw(2) << inventory[SAUCE];
    cout << " 番:" << setw(2) << inventory[KETCHUP];
    cout << " 薯配:" << setw(2) << inventory[FRIES_ING] << "\n";
    cout << "[耗料] 纸:" << setw(2) << inventory[WRAPPER];
    cout << " 盒:" << setw(2) << inventory[FRIES_BOX];
    cout << " 杯:" << setw(2) << inventory[COLA_CUP] << "\n";
    cout << "[饮料] 薯条:" << setw(2) << inventory[FRIES];
    cout << " 可乐液:" << setw(2) << inventory[COLA] << "\n";
    cout << "----------------------------------------\n";
}

// 显示帮助信息
void showHelp() {
    cout << "\n指令帮助:\n";
    cout << "卷饼制作: [1]放饼 [2]加肉 [3]加黄瓜 [4]加沙司 [5]薯条料 [6]加番茄酱\n";
    cout << "          [7]卷起 [8]包装 [9]给顾客\n";
    cout << "小吃制作: [a]拿盒 [b]装薯条 [c]给顾客(薯条)\n";
    cout << "          [d]拿杯 [e]接可乐 [f]给顾客(可乐)\n";
    cout << "库存管理: [z]切肉 [x]炸薯条\n";
    cout << "          [r]补面饼/纸 [t]补配料 [y]补盒子/杯子\n";
    cout << "重置功能: [`]重置卷饼台 [~]重置小吃台\n";  // 新增
    cout << "其他:     [0]结束本日 [h]显示帮助\n";
}
// 更新游戏状态
void updateGame() {
    if (currentTime > 0) {
        currentTime--;
    }

    // 更新顾客耐心
    for (int i = 0; i < customerCount; i++) {
        if (customerPatience[i] > 0) {
            customerPatience[i]--;
            if (customerPatience[i] <= 0) {
                cout << ">> 提示: 顾客" << i + 1 << "等不及离开了！\n";
                // 移除顾客
                for (int j = i; j < customerCount - 1; j++) {
                    customerPatience[j] = customerPatience[j + 1];
                    customerWantsBurrito[j] = customerWantsBurrito[j + 1];
                    customerWantsFries[j] = customerWantsFries[j + 1];
                    customerWantsCola[j] = customerWantsCola[j + 1];
                    for (int k = 0; k < 5; k++) {
                        customerBurritoNeed[k][j] = customerBurritoNeed[k][j + 1];
                    }
                }
                customerCount--;
                i--;
            }
        }
    }

    // 自动生成新顾客
    int maxCustomers = (shopLevel & 4) ? 3 : 1;
    if (customerCount < maxCustomers && rand() % 100 < 30) {
        generateCustomer();
    }

    // 自动切肉机功能
    if ((shopLevel & 2) && inventory[MEAT] == 0) {
        inventory[MEAT] = INVENTORY_MAX;
        cout << ">> 提示: 自动切肉机补满了肉库存！\n";
    }
}

// 生成顾客需求
void generateCustomer() {
    int maxCustomers = (shopLevel & 4) ? 3 : 1;
    if (customerCount >= maxCustomers) return;

    int idx = customerCount;
    customerPatience[idx] = MAX_PATIENCE;

    // 随机生成需求
    customerWantsBurrito[idx] = rand() % 2;  // 50%要卷饼
    customerWantsFries[idx] = rand() % 2;    // 50%要薯条
    customerWantsCola[idx] = rand() % 2;     // 50%要可乐

    // 确保至少有一个需求
    if (!customerWantsBurrito[idx] && !customerWantsFries[idx] && !customerWantsCola[idx]) {
        customerWantsBurrito[idx] = 1;
    }

    // 卷饼需求
    for (int i = 0; i < 5; i++) {
        customerBurritoNeed[i][idx] = 0;
    }
    if (customerWantsBurrito[idx]) {
        int ingCount = rand() % 5 + 1;  // 1-5种配料
        for (int i = 0; i < ingCount; i++) {
            int ing = rand() % 5;
            customerBurritoNeed[ing][idx] = 1;
        }
    }

    customerCount++;
    cout << ">> 提示: 新顾客来了！\n";
}