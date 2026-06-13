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
void deleteProduct() {
    showProducts();
    int id;
    cout << "输入要下架的商品ID: ";
    cin >> id;
    auto it = remove_if(products.begin(), products.end(), [id](const Product& p) { return p.id == id; });
    if (it != products.end()) {
        products.erase(it, products.end());
        saveProducts();
        cout << "商品已下架！\n";
    }
    else {
        cout << "商品不存在！\n";
    }
}

// ====================== 购物车 ======================
void addToCart() {
    if (!currentUser) { cout << "请先登录！\n"; return; }
    showProducts();
    int id, qty;
    cout << "\n商品ID: "; cin >> id;
    cout << "数量: "; cin >> qty;

    for (auto& p : products) {
        if (p.id == id) {
            if (p.stock < qty) { cout << "库存不足！\n"; return; }
            for (auto& item : currentCart) {
                if (item.productId == id) {
                    item.quantity += qty;
                    cout << "购物车数量已更新！\n";
                    return;
                }
            }
            currentCart.push_back({ id, p.name, p.price, qty });
            cout << "已加入购物车！\n";
            return;
        }
    }
    cout << "商品不存在！\n";
}

void showCart() {
    if (currentCart.empty()) { cout << "购物车为空！\n"; return; }
    cout << "\n=== 我的购物车 ===\n";
    double total = 0;
    cout << setw(5) << "ID" << setw(20) << "商品" << setw(10) << "单价"
        << setw(8) << "数量" << setw(12) << "小计" << endl;
    cout << string(65, '-') << endl;
    for (const auto& item : currentCart) {
        double sub = item.price * item.quantity;
        total += sub;
        cout << setw(5) << item.productId << setw(20) << item.productName
            << setw(10) << item.price << setw(8) << item.quantity << setw(12) << sub << endl;
    }
    cout << "\n总计: " << fixed << setprecision(2) << total << " 元\n";
}

void modifyCartItem() {
    if (currentCart.empty()) {
        cout << "购物车为空！\n";
        return;
    }

    showCart();

    int id, newQty;
    cout << "\n请输入要修改的商品ID: ";
    cin >> id;
    cout << "请输入新数量 (输入0或负数将删除该商品): ";
    cin >> newQty;

    // 查找并处理
    auto it = currentCart.begin();
    while (it != currentCart.end()) {
        if (it->productId == id) {
            if (newQty <= 0) {
                cout << "已删除商品: " << it->productName << endl;
                it = currentCart.erase(it);   // 删除该商品
            } else {
                it->quantity = newQty;
                cout << "数量修改成功！" << endl;
                ++it;
            }
            return;   // 找到后立即退出
        }
        ++it;
    }

    cout << "购物车中未找到该商品！\n";
}
void removeFromCart() {
    showCart();
    if (currentCart.empty()) return;
    int id;
    cout << "输入要删除的商品ID: "; cin >> id;
    auto it = remove_if(currentCart.begin(), currentCart.end(), [id](const CartItem& c) { return c.productId == id; });
    currentCart.erase(it, currentCart.end());
    cout << "已删除！\n";
}
void clearCart() {
    currentCart.clear();
    cout << "购物车已清空！\n";
}

// ====================== 订单管理 ======================
void checkout() {
    if (currentCart.empty()) { cout << "购物车为空！\n"; return; }
    showCart();
    cout << "\n确认下单？(y/n): ";
    char c; cin >> c;
    if (c != 'y' && c != 'Y') return;

    Order order;
    order.orderId = nextOrderId++;
    order.username = currentUser->username;
    order.items = currentCart;
    order.totalAmount = 0;
    for (auto& item : order.items) order.totalAmount += item.price * item.quantity;
    order.status = "待付款";
    order.orderTime = getCurrentTime();
    order.address = currentUser->address;

    orders.push_back(order);

    // 扣库存
    for (auto& item : currentCart) {
        for (auto& p : products) {
            if (p.id == item.productId) {
                p.stock -= item.quantity;
                break;
            }
        }
    }
    currentCart.clear();
    saveProducts();
    cout << "下单成功！订单号: " << order.orderId << endl;
}

void showMyOrders() {
    cout << "\n=== 我的订单 ===\n";
    bool has = false;
    for (const auto& o : orders) {
        if (o.username == currentUser->username) {
            has = true;
            cout << "订单号: " << o.orderId << "  金额: " << o.totalAmount
                << "  状态: " << o.status << "  时间: " << o.orderTime << endl;
        }
    }
    if (!has) cout << "暂无订单。\n";
}

void showOrderDetail(int orderId) {
    for (const auto& o : orders) {
        if (o.orderId == orderId && o.username == currentUser->username) {
            cout << "\n=== 订单详情 ===\n订单号: " << o.orderId << endl;
            cout << "时间: " << o.orderTime << endl;
            cout << "地址: " << o.address << endl;
            cout << "状态: " << o.status << endl;
            cout << "商品列表:\n";
            for (const auto& item : o.items) {
                cout << "  " << item.productName << " x" << item.quantity << "  小计:"
                    << item.price * item.quantity << endl;
            }
            cout << "总金额: " << o.totalAmount << " 元\n";
            return;
        }
    }
    cout << "订单不存在！\n";
}
// ====================== 模拟支付 ======================
void simulatePayment(int orderId) {
    if (!currentUser) {
        cout << "请先登录！\n";
        return;
    }

    for (auto& o : orders) {
        if (o.orderId == orderId && o.username == currentUser->username) {
            if (o.status != "待付款") {
                cout << "该订单当前状态为：" << o.status << "，无法支付！\n";
                return;
            }

            cout << "\n=== 模拟支付 ===\n";
            cout << "订单号: " << orderId << "  总金额: " << o.totalAmount << " 元\n";
            cout << "请输入支付密码 (6位数字): ";
            string payPwd;
            cin >> payPwd;

            if (payPwd.length() >= 4) {   // 简单验证
                o.status = "已支付";
                cout << "支付成功！感谢您的购买。\n";
            }
            else {
                cout << "支付密码过短，支付失败。\n";
            }
            return;
        }
    }
    cout << "未找到该订单！\n";
}
// ====================== 取消订单 ======================
void cancelOrder(int orderId) {
    if (!currentUser) {
        cout << "请先登录！\n";
        return;
    }

    for (auto& o : orders) {
        if (o.orderId == orderId && o.username == currentUser->username) {
            if (o.status == "待付款" || o.status == "已取消") {

                // ==================== 恢复库存 ====================
                for (const auto& item : o.items) {
                    for (auto& p : products) {
                        if (p.id == item.productId) {
                            p.stock += item.quantity;   // 恢复库存
                            break;
                        }
                    }
                }
                // ================================================

                if (o.status != "已取消") {
                    o.status = "已取消";
                    saveProducts();   // 保存更新后的库存
                    cout << "订单已成功取消！\n";
                }
                else {
                    cout << "该订单已取消。\n";
                }
            }
            else {
                cout << "只有【待付款】状态的订单才能取消，当前状态：" << o.status << endl;
            }
            return;
        }
    }
    cout << "未找到该订单！\n";
}

// ====================== 确认收货 ======================
void confirmReceipt(int orderId) {
    if (!currentUser) {
        cout << "请先登录！\n";
        return;
    }

    for (auto& o : orders) {
        if (o.orderId == orderId && o.username == currentUser->username) {
            if (o.status == "已支付") {
                o.status = "已完成";
                cout << "确认收货成功！订单已完成。\n";
            }
            else if (o.status == "已完成") {
                cout << "该订单已完成。\n";
            }
            else {
                cout << "当前状态为：" << o.status << "，无法确认收货。\n";
            }
            return;
        }
    }
    cout << "未找到该订单！\n";
}
// ====================== 管理员菜单 ======================
void adminMenu() {
    while (true) {
        clearScreen();
        cout << "========== 管理员后台 ==========\n";
        cout << "1. 添加商品\n";
        cout << "2. 修改商品\n";
        cout << "3. 下架商品\n";
        cout << "4. 查看所有商品\n";
        cout << "5. 返回\n";
        cout << "请选择: ";
        int choice; cin >> choice;
        if (choice == 1) addProduct();
        else if (choice == 2) modifyProduct();
        else if (choice == 3) deleteProduct();
        else if (choice == 4) showProducts();
        else if (choice == 5) break;
        else cout << "输入错误！\n";
        pause();
    }
}