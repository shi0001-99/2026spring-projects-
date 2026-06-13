#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <limits>

using namespace std;

// ====================== 数据结构 ======================
struct User {
    string username;
    string password;
    string phone;
    string address;
    bool isAdmin = false;
};

struct Product {
    int id;
    string name;
    string category;
    double price;
    int stock;
    string description;
};

struct CartItem {
    int productId;
    string productName;
    double price;
    int quantity;
};

struct Order {
    int orderId;
    string username;
    vector<CartItem> items;
    double totalAmount;
    string status;        // 待付款、已支付、已发货、已完成、已取消
    string orderTime;
    string address;
};

// ====================== 全局变量 ======================
vector<User> users;
vector<Product> products;
vector<Order> orders;
vector<CartItem> currentCart;
User* currentUser = nullptr;
int nextProductId = 1;
int nextOrderId = 1;

// ====================== 辅助函数 ======================
void clearScreen() {
    cout << "\033[2J\033[1;1H";
}

void pause() {
    cout << "\n按回车键继续...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

string getCurrentTime() {
    time_t now = time(nullptr);
    char buf[80];
#ifdef _WIN32
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
#else
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
#endif
    return string(buf);
}