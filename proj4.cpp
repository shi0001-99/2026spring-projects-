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
// ====================== 文件操作 ======================
void loadUsers();
void saveUsers();
void loadProducts();
void saveProducts();
void loadOrders();
void saveOrders();
void loadCart();
void saveCart();

void loadDefaultProducts() {
    if (!products.empty()) return;

    products = {
        {nextProductId++, "华为Mate 60", "数码", 5999.0, 50, "麒麟芯片 旗舰手机"},
        {nextProductId++, "小米纯棉T恤", "服装", 89.0, 200, "舒适透气 经典款"},
        {nextProductId++, "雀巢速溶咖啡", "食品", 45.0, 300, "香浓提神"},
        {nextProductId++, "欧莱雅水光面霜", "美妆", 199.0, 80, "补水保湿"},
        {nextProductId++, "苹果 AirPods", "数码", 1299.0, 60, "无线耳机"}
    };
    saveProducts();   // 确保保存正确格式
}

// ====================== 用户管理 ======================
void registerUser() {
    User u; 
    cout << "=== 用户注册 ===\n";
    cout << "用户名: "; cin >> u.username;
    for (const auto& user : users) {
        if (user.username == u.username) {
            cout << "用户名已存在！\n";
            return;
        }
    }
    cout << "密码: "; cin >> u.password;
    cout << "手机号: "; cin >> u.phone;
    cin.ignore();
    cout << "收货地址: "; getline(cin, u.address);

    users.push_back(u);
    saveUsers();
    cout << "注册成功！\n";
}

bool login() {
    string username, password;
    cout << "=== 用户登录 ===\n";
    cout << "用户名: "; cin >> username;
    cout << "密码: "; cin >> password;

    for (auto& u : users) {
        if (u.username == username && u.password == password) {
            currentUser = &u;
            cout << "登录成功！欢迎 " << username << "!\n";
            loadCart();
            return true;
        }
    }
    cout << "用户名或密码错误！\n";
    return false;
}

void showProfile() {
    if (!currentUser) return;
    cout << "\n=== 个人信息 ===\n";
    cout << "用户名: " << currentUser->username << endl;
    cout << "手机号: " << currentUser->phone << endl;
    cout << "收货地址: " << currentUser->address << endl;
}

void modifyProfile() {
    if (!currentUser) return;
    cout << "\n=== 修改个人信息 ===\n";
    cin.ignore();
    cout << "新手机号 (回车跳过): ";
    string temp;
    getline(cin, temp);
    if (!temp.empty()) currentUser->phone = temp;

    cout << "新收货地址 (回车跳过): ";
    getline(cin, temp);
    if (!temp.empty()) currentUser->address = temp;

    saveUsers();
    cout << "修改成功！\n";
}

void logout() {
    if (currentUser) {
        saveCart();
        cout << "已退出登录，再见 " << currentUser->username << "!\n";
    }
    currentUser = nullptr;
    currentCart.clear();
}