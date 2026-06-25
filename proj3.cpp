#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <algorithm>
#include <windows.h>
#include <tchar.h>          

using namespace std;


string Tstr_to_str(const TCHAR* tstr) {
    if (!tstr) return "";

#ifdef _UNICODE
    
    int len = WideCharToMultiByte(CP_ACP, 0, tstr, -1, NULL, 0, NULL, NULL);
    string str(len, 0);
    WideCharToMultiByte(CP_ACP, 0, tstr, -1, &str[0], len, NULL, NULL);
    if (!str.empty() && str.back() == '\0') str.pop_back();
    return str;
#else
    
    return string(tstr);
#endif
}


struct Word {
    string eng;
    string chn;
};

struct User {
    string username;
    string password;
};

// 全局变量
vector<Word> elemWords, juniorWords, seniorWords;
vector<Word> currentList;
vector<Word> wrongList;
map<string, vector<Word>> userWrongBooks;

string currentUser = "";
const string USERS_FILE = "users.txt";


void loadWords(const string& filename, vector<Word>& words)折叠区域

void loadAllWords()折叠区域

vector<User> loadUsers()折叠区域

void saveUser(const string& username, const string& password)折叠区域


void loadUserWrongBook(const string& username) {
    if (username.empty()) return;

    string filename = username + "_wrong.txt";
    ifstream file(filename);

    wrongList.clear();
    userWrongBooks[username].clear();   

    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        size_t pos = line.find('\t');
        if (pos != string::npos) {
            Word w = { line.substr(0, pos), line.substr(pos + 1) };
            wrongList.push_back(w);
            userWrongBooks[username].push_back(w);   
        }
    }
    file.close();
}

void saveUserWrongBook(const string& username) {
    if (username.empty()) return;

    string filename = username + "_wrong.txt";
    ofstream file(filename);
    if (!file.is_open()) return;

   
    for (const auto& w : userWrongBooks[username]) {
        file << w.eng << "\t" << w.chn << endl;
    }
    file.close();
}


bool login(const string& username, const string& password) {
    auto users = loadUsers();
    for (const auto& u : users) {
        if (u.username == username && u.password == password) {
            currentUser = username;
            loadUserWrongBook(username);
            return true;
        }
    }
    return false;
}

bool registerUser(const string& username, const string& password) {
    if (username.empty() || password.empty()) return false;
    auto users = loadUsers();
    for (const auto& u : users) if (u.username == username) return false;

    saveUser(username, password);
    currentUser = username;
    return true;
}


void drawButton(int x, int y, int w, int h, const TCHAR* text,
    COLORREF btnColor = RGB(0, 102, 204),
    COLORREF textColor = WHITE)折叠区域

bool isClick(int mx, int my, int x, int y, int w, int h) {
    return mx >= x && mx <= x + w && my >= y && my <= y + h;
}

int mainMenu() {
    initgraph(800, 600);
    setbkcolor(RGB(245, 245, 245));

    BeginBatchDraw();        

    while (true) {
        cleardevice();

        settextcolor(BLACK);
        settextstyle(52, 0, _T("宋体"));
        outtextxy(240, 70, _T("背单词系统"));

        drawButton(250, 180, 300, 70, _T("开始背单词"), RGB(0, 102, 204), BLACK);
        drawButton(250, 270, 300, 70, _T("我的错题本"), RGB(0, 102, 204), BLACK);
        drawButton(250, 360, 300, 70, _T("退出登录"), RGB(0, 102, 204), BLACK);
        drawButton(250, 450, 300, 70, _T("退出程序"), RGB(0, 102, 204), BLACK);

        FlushBatchDraw();   

        ExMessage msg = getmessage(EX_MOUSE);
        if (msg.message == WM_LBUTTONDOWN) {
            int mx = msg.x, my = msg.y;
            if (isClick(mx, my, 250, 180, 300, 70)) return 1;
            if (isClick(mx, my, 250, 270, 300, 70)) return 2;
            if (isClick(mx, my, 250, 360, 300, 70)) return 3;
            if (isClick(mx, my, 250, 450, 300, 70)) return 4;
        }
    }
}


void studySession(bool isRandom, bool engToChn) {
    if (currentList.empty()) {
        MessageBox(GetHWnd(), _T("当前没有单词！"), _T("提示"), MB_OK);
        return;
    }

    vector<int> indices(currentList.size());
    for (int i = 0; i < currentList.size(); ++i) indices[i] = i;

    if (isRandom) {
        srand((unsigned)time(0));
        random_shuffle(indices.begin(), indices.end());
    }

    BeginBatchDraw();

    for (size_t i = 0; i < indices.size(); ++i) {
        cleardevice();

        int idx = indices[i];
        const Word& w = currentList[idx];

        settextcolor(BLACK);
        settextstyle(28, 0, _T("宋体"));
        outtextxy(50, 40, (_T("进度: ") + to_string(i + 1) + _T("/") + to_string(indices.size())).c_str());

        // 生成选项
        vector<string> options;
        options.push_back(engToChn ? w.chn : w.eng);

        vector<int> wrongIdx;
        for (int j = 0; j < currentList.size() && wrongIdx.size() < 30; ++j) {
            if (j == idx) continue;
            wrongIdx.push_back(j);
        }
        random_shuffle(wrongIdx.begin(), wrongIdx.end());

        for (int j = 0; j < 3 && j < wrongIdx.size(); ++j) {
            options.push_back(engToChn ? currentList[wrongIdx[j]].chn : currentList[wrongIdx[j]].eng);
        }
        random_shuffle(options.begin(), options.end());

        // 显示题目
        settextstyle(30, 0, _T("宋体"));
        if (engToChn) {
            outtextxy(80, 120, (_T("请选择 “") + w.eng + _T("” 的中文意思：")).c_str());
        }
        else {
            outtextxy(80, 120, (_T("请选择 “") + w.chn + _T("” 的英文：")).c_str());
        }

        int btnY[4] = { 220, 290, 360, 430 };
        int correctIndex = -1;
        for (int j = 0; j < 4; ++j) {
            if (options[j] == (engToChn ? w.chn : w.eng)) correctIndex = j;
            drawButton(140, btnY[j], 520, 58, options[j].c_str(), RGB(0, 102, 204), BLACK);
        }

        FlushBatchDraw();

        // 等待选择
        bool answered = false;
        while (!answered) {
            ExMessage msg = getmessage(EX_MOUSE);
            if (msg.message == WM_LBUTTONDOWN) {
                for (int j = 0; j < 4; ++j) {
                    if (isClick(msg.x, msg.y, 140, btnY[j], 520, 58)) {
                        bool isCorrect = (j == correctIndex);

                        if (isCorrect) {
                            settextcolor(GREEN);
                            outtextxy(200, 510, _T("正确！"));
                        }
                        else {
                            settextcolor(RED);
                            outtextxy(200, 510, _T("错误！"));

                         
                            bool alreadyExist = false;
                            for (const auto& ew : userWrongBooks[currentUser]) {
                                if (ew.eng == w.eng) {
                                    alreadyExist = true;
                                    break;
                                }
                            }
                            if (!alreadyExist) {
                                userWrongBooks[currentUser].push_back(w);
                                saveUserWrongBook(currentUser);   // 立即保存到文件
                            }
                           
                        }

                        answered = true;
                        break;
                    }
                }
            }
        }

        // 显示正确答案
        settextcolor(BLACK);
        outtextxy(180, 555, (_T("正确答案：") + (engToChn ? w.chn : w.eng)).c_str());
        settextcolor(BLUE);
      
        FlushBatchDraw();

        while (true) {
            ExMessage msg = getmessage(EX_MOUSE | EX_KEY);
            if (msg.message == WM_LBUTTONDOWN || msg.message == WM_KEYDOWN) break;
        }
    }

    EndBatchDraw();
    MessageBox(GetHWnd(), _T("本次背诵结束！"), _T("完成"), MB_OK);
}

int selectRange() {
    BeginBatchDraw();                    // 开启双缓冲
    setbkcolor(RGB(245, 245, 245));

    while (true) {
        cleardevice();

        settextcolor(BLACK);
        settextstyle(40, 0, _T("宋体"));
        outtextxy(280, 80, _T("选择单词范围"));

        drawButton(250, 170, 300, 70, _T("小学单词"), RGB(0, 102, 204), BLACK);
        drawButton(250, 260, 300, 70, _T("初中单词"), RGB(0, 102, 204), BLACK);
        drawButton(250, 350, 300, 70, _T("高中单词"), RGB(0, 102, 204), BLACK);
        drawButton(250, 450, 300, 60, _T("返回主菜单"), RGB(0, 102, 204), BLACK);

        FlushBatchDraw();                // 必须刷新画面

        ExMessage msg = getmessage(EX_MOUSE);
        if (msg.message == WM_LBUTTONDOWN) {
            int x = msg.x, y = msg.y;

            if (isClick(x, y, 250, 170, 300, 70)) { EndBatchDraw(); return 1; } // 小学
            if (isClick(x, y, 250, 260, 300, 70)) { EndBatchDraw(); return 2; } // 初中
            if (isClick(x, y, 250, 350, 300, 70)) { EndBatchDraw(); return 3; } // 高中
            if (isClick(x, y, 250, 450, 300, 60)) { EndBatchDraw(); return 0; } // 返回
        }
    }
}

void selectStudyMode(bool& isRandom, bool& engToChn) {
    setbkcolor(RGB(245, 245, 245));
    BeginBatchDraw();                    

    bool selectedRandom = false;
    bool selectedEngToChn = true;

    while (true) {
        cleardevice();

        settextcolor(BLACK);
        settextstyle(45, 0, _T("宋体"));
        outtextxy(290, 70, _T("背诵设置"));

        // ================ 顺序 / 随机 ================
        drawButton(220, 170, 360, 70, _T("顺序背诵"),
            selectedRandom ? RGB(0, 102, 204) : RGB(0, 153, 0), BLACK);

        drawButton(220, 260, 360, 70, _T("随机背诵"),
            selectedRandom ? RGB(0, 153, 0) : RGB(0, 102, 204), BLACK);

        // ================ 英中 / 中英 ================
        drawButton(200, 360, 180, 65, _T("英 → 中"),
            selectedEngToChn ? RGB(0, 153, 0) : RGB(0, 102, 204), BLACK);

        drawButton(420, 360, 180, 65, _T("中 → 英"),
            selectedEngToChn ? RGB(0, 102, 204) : RGB(0, 153, 0), BLACK);

        drawButton(280, 460, 240, 65, _T("开始背诵"), RGB(34, 139, 34), BLACK);

        FlushBatchDraw();   // 刷新画面

        // ================ 鼠标点击处理 ================
        ExMessage msg = getmessage(EX_MOUSE);
        if (msg.message == WM_LBUTTONDOWN) {
            int x = msg.x, y = msg.y;

            // 顺序 / 随机选择
            if (isClick(x, y, 220, 170, 360, 70)) selectedRandom = false;
            if (isClick(x, y, 220, 260, 360, 70)) selectedRandom = true;

            // 方向选择
            if (isClick(x, y, 200, 360, 180, 65)) selectedEngToChn = true;
            if (isClick(x, y, 420, 360, 180, 65)) selectedEngToChn = false;

            // 开始背诵
            if (isClick(x, y, 280, 460, 240, 65)) {
                isRandom = selectedRandom;
                engToChn = selectedEngToChn;
                EndBatchDraw();
                return;
            }
        }
    }
}
int main() {
    loadAllWords();

    initgraph(640, 480);
    setbkcolor(RGB(240, 248, 255));

    bool logged = false;
    while (!logged) {
        cleardevice();
        settextstyle(42, 0, _T("宋体"));
        settextcolor(BLACK);                    
        outtextxy(220, 70, _T("用户登录"));

        drawButton(200, 160, 240, 60, _T("登录"), RGB(0, 102, 204), BLACK);
        drawButton(200, 240, 240, 60, _T("注册"), RGB(0, 102, 204), BLACK);
        drawButton(200, 320, 240, 60, _T("退出"), RGB(0, 102, 204), BLACK);

        ExMessage msg;
        while (true) {
            msg = getmessage(EX_MOUSE);
            if (msg.message == WM_LBUTTONDOWN) {
                int mx = msg.x, my = msg.y;

                if (isClick(mx, my, 200, 160, 240, 60)) { // 登录
                    TCHAR u[64] = { 0 }, p[64] = { 0 };
                    InputBox(u, 60, _T("请输入用户名"), _T("登录"));
                    InputBox(p, 60, _T("请输入密码"), _T("登录"));
                    if (login(Tstr_to_str(u), Tstr_to_str(p))) logged = true;
                    else MessageBox(GetHWnd(), _T("用户名或密码错误！"), _T("登录失败"), MB_OK);
                }
                else if (isClick(mx, my, 200, 240, 240, 60)) { // 注册
                    TCHAR u[64] = { 0 }, p[64] = { 0 };
                    InputBox(u, 60, _T("请输入新用户名"), _T("注册"));
                    InputBox(p, 60, _T("请输入密码"), _T("注册"));
                    if (registerUser(Tstr_to_str(u), Tstr_to_str(p))) logged = true;
                    else MessageBox(GetHWnd(), _T("用户名已存在！"), _T("注册失败"), MB_OK);
                }
                else if (isClick(mx, my, 200, 320, 240, 60)) {
                    closegraph();
                    return 0;
                }
                break;
            }
        }
    }

    // 主循环
    while (true) {
        int choice = mainMenu();

        if (choice == 1) {   // 开始背单词
            int range = selectRange();     // 调用新函数
            if (range == 0) continue;

            if (range == 1) currentList = elemWords;
            else if (range == 2) currentList = juniorWords;
            else if (range == 3) currentList = seniorWords;

            // 选择背诵模式
            bool isRandom = false, engToChn = true;
            selectStudyMode(isRandom, engToChn);

            studySession(isRandom, engToChn);
        }
        else if (choice == 2) {  // 我的错题本
            if (currentUser.empty()) {
                MessageBox(GetHWnd(), _T("请先登录！"), _T("提示"), MB_OK);
                continue;
            }

            loadUserWrongBook(currentUser);
            vector<Word> toReview = userWrongBooks[currentUser];
            if (toReview.empty()) toReview = wrongList;

            if (toReview.empty()) {
                MessageBox(GetHWnd(), _T("暂无错题记录！\n先去背单词吧~"), _T("错题本"), MB_OK);
                continue;
            }

           
            currentList = toReview;

            bool isRandom = false;
            bool engToChn = true;

            
            selectStudyMode(isRandom, engToChn);

            // 开始错题复习
            studySession(isRandom, engToChn);
           
        }
        else if (choice == 3) {
            currentUser = "";
            closegraph();
            main();
            return 0;
        }
        else if (choice == 4) {
            closegraph();
            break;
        }
    }
    return 0;
}
