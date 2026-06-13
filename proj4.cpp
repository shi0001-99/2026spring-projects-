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
// ====================== 商品管理 ======================
void showProducts() {
    cout << "\n=== 商品列表 ===\n";
    cout << setw(5) << "ID" << setw(20) << "名称" << setw(12) << "分类"
        << setw(10) << "价格" << setw(8) << "库存" << endl;
    cout << string(65, '-') << endl;
    for (const auto& p : products) {
        cout << setw(5) << p.id << setw(20) << p.name << setw(12) << p.category
            << setw(10) << fixed << setprecision(2) << p.price
            << setw(8) << p.stock << endl;
    }
}

void showProductDetail(int id) {
    for (const auto& p : products) {
        if (p.id == id) {
            cout << "\n=== 商品详情 ===\n";
            cout << "名称: " << p.name << endl;
            cout << "分类: " << p.category << endl;
            cout << "价格: " << p.price << " 元" << endl;
            cout << "库存: " << p.stock << endl;
            cout << "描述: " << p.description << endl;
            return;
        }
    }
    cout << "未找到该商品！\n";
}

void searchProducts() {
    string keyword;
    cout << "请输入搜索关键词: ";
    cin.ignore();
    getline(cin, keyword);
    cout << "\n=== 搜索结果 ===\n";
    bool found = false;
    for (const auto& p : products) {
        if (p.name.find(keyword) != string::npos || p.description.find(keyword) != string::npos) {
            cout << p.id << ". " << p.name << " - " << p.price << "元\n";
            found = true;
        }
    }
    if (!found) cout << "未找到匹配商品。\n";
}

void filterByCategory() {
    string cat;
    cout << "请输入分类 (数码/服装/食品/美妆): ";
    cin >> cat;
    cout << "\n=== " << cat << " 分类商品 ===\n";
    bool found = false;
    for (const auto& p : products) {
        if (p.category == cat) {
            cout << p.id << ". " << p.name << " - " << p.price << "元 (库存:" << p.stock << ")\n";
            found = true;
        }
    }
    if (!found) cout << "该分类暂无商品。\n";
}

void addProduct() {
    Product p;
    p.id = nextProductId++;
    cout << "商品名称: "; cin.ignore(); getline(cin, p.name);
    cout << "分类: "; getline(cin, p.category);
    cout << "价格: "; cin >> p.price;
    cout << "库存: "; cin >> p.stock;
    cin.ignore();
    cout << "描述: "; getline(cin, p.description);
    products.push_back(p);
    saveProducts();
    cout << "商品添加成功！\n";
}

void modifyProduct() {
    showProducts();
    if (products.empty()) {
        cout << "当前没有商品！\n";
        return;
    }

    int id;
    cout << "\n请输入要修改的商品ID: ";
    cin >> id;

    for (auto& p : products) {
        if (p.id == id) {
            cout << "\n=== 当前商品信息 ===\n";
            cout << "名称: " << p.name << endl;
            cout << "分类: " << p.category << endl;
            cout << "价格: " << p.price << " 元" << endl;
            cout << "库存: " << p.stock << endl;
            cout << "描述: " << p.description << endl;

            cout << "\n=== 修改商品信息（回车跳过不修改） ===\n";

            cin.ignore();
            cout << "新名称: ";
            string temp;
            getline(cin, temp);
            if (!temp.empty()) p.name = temp;

            cout << "新分类: ";
            getline(cin, temp);
            if (!temp.empty()) p.category = temp;

            cout << "新价格: ";
            getline(cin, temp);
            if (!temp.empty()) p.price = stod(temp);

            cout << "新库存（可增加数量）: ";
            getline(cin, temp);
            if (!temp.empty()) p.stock = stoi(temp);

            cout << "新描述: ";
            getline(cin, temp);
            if (!temp.empty()) p.description = temp;

            saveProducts();
            cout << "\n商品信息修改成功！\n";
            return;
        }
    }
    cout << "未找到ID为 " << id << " 的商品！\n";
}