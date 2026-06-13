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

