#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <signal.h>
#include <algorithm>
#include <stdio.h>

#include "function.h"

using namespace std;

/*
    Hien bang huong dan
*/
void _help() {
    cout << color[2];
    cout <<   "############################\n";
    slow_type("WELCOME TO OUR SHELL PROJECT\n");
    cout <<   "############################\n";
    slow_type("Here is the list of instructions:\n\n");
    int run = 0;
    for (int i = 0; i < NUMBER_OF_COMMANDS; ++ i) {
        cout << color[run] << cmdLst[i] << "\t-\t" << instuctionLst[i] << defaultCor << "\n";
        run = (run + 1) % 6;
    }
    cout << defaultCor;
    return;
}

/*
    Thoat chuong trinh tinyShell
*/
void _exit() {
    cout << color[0];
    cout << "*** Exit tinyShell ***" << "\n";
    cout << defaultCor;
    //Doi 3s de nguoi dung doc duoc *** Exit tinyShell ***
    Sleep(3000);
    
    shellStatus = false;
    // Dong tat ca cac process/thread dang thuc hien
    for (int i = (int) listProcess.size() - 1; i >= 0; -- i) {
        TerminateProcess(listProcess[i].pi.hProcess, 0);
        CloseHandle(listProcess[i].pi.hProcess);
        CloseHandle(listProcess[i].pi.hThread);
    }
}

/*
    Ham tra lai va thay doi thoi gian hien tai
*/
void _time() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    printf("Current time: %02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond);
    printf("Current date: %02d/%02d/%04d\n", st.wDay, st.wMonth, st.wYear);

int hour, minute, second;
    printf("Enter new time: ");
    if(scanf("%d:%d:%d", &hour,&minute,&second)==3){
        st.wHour = hour;
        st.wMinute = minute;
        st.wSecond = second;

        if(SetLocalTime(&st)){
            printf("Time successfully updated.\n");
        }
        else{
            printf("Failed to updated time. Error code: %d\n", GetLastError());
        }
    }

    int day,month,year;
    printf("Enter new date (dd/mm/yyyy): ");
    if (scanf("%d/%d/%d", &day, &month, &year) == 3) {
        st.wDay = day;
        st.wMonth = month;
        st.wYear = year;

        if (SetLocalTime(&st)) {
            printf("Date successfully updated.\n");
        } else {
            printf("Failed to update date. Error code: %d\n", GetLastError());
        }
    } else {
        printf("Invalid date format. Please use dd/mm/yyyy.\n");
    }

}


/*
    Ham tra lai path hien tai
*/
string getPath() {
    char buff[MAX_PATH];
    int n = GetCurrentDirectory(MAX_PATH, buff);
    if (n) return buff;
    return "";
}

/*
    Liet ke file va subdirectories
*/
void _dir(string inst) {
    vector<string> args = split_space(inst);
    string path;
    if (sz(args) < 2) path = getPath();
    else path = args[1];
    cout << color[2];
    cout << "\t Directory: " << path << "\n\n";
    path = path + "\\" + "*";
    WIN32_FIND_DATA data;
    HANDLE hFind = FindFirstFile(path.c_str(), &data);
    if (hFind != INVALID_HANDLE_VALUE) {
        cout << "\n";
        cout << "\tLastWriteTime\t\tLength\t\tName" << "\n";
        cout << "\t-------------\t\t------\t\t----" << "\n";
        SYSTEMTIME tmpTime;
        do {
            FileTimeToSystemTime(&data.ftLastWriteTime, &tmpTime);
            char date[100];
            snprintf(date, 30, "     %02d/%02d/%04d  %02d:%02d:%02d", tmpTime.wDay, tmpTime.wMonth, tmpTime.wYear, tmpTime.wHour, tmpTime.wMinute, tmpTime.wSecond);
            string str(date);
            cout << setw(32) << left << str;
            cout << setw(15) << left << data.nFileSizeLow;
            cout << setw(20) << left << data.cFileName;
            cout << "\n";
        } while (FindNextFile(hFind, &data));
        FindClose(hFind);
    } else {
        cout << errorMsg << "Unable to access directory" << defaultCor << "\n";
    }
    cout << defaultCor;
}

/*
    Ham nay goi den ham getPath()
*/
void _pwd() {
    cout << color[2];
    cout << "Current working directory: " << getPath() << "\n";
    cout << defaultCor;
}

/*
    Clear man hinh
*/
void _clear() {
    system("cls");
}


/*
    In ra file yeu cau, can duong dan day du hoac file nam trong duong dan hien tai
*/
void _cat(string inst) {
    vector<string> args = split_space(inst);
    if (sz(args) != 2) {
        cout << errorMsg << "Invalid option" << defaultCor << "\n";
        cout << usageMsg << "cat <path-to-file>" << defaultCor << "\n";
        return;
    }
    string path = args[1];
    ifstream myFile(path);
    if (myFile.is_open()) {
        string line;
        cout << color[4];
        while(getline(myFile, line)) {
            cout << line << "\n";
        }
        cout << defaultCor;
        myFile.close();
    } else {
        cout << errorMsg << "Unable to open file" << defaultCor << "\n";
    }
}

/*
    Chay tung dong trong file .bat
*/
void runDotBat(string fileName) {
    ifstream myFile(fileName);
    if (myFile.is_open()) {
        string line;
        while(getline(myFile, line)) {
            line = remove_space(line);
            shell_execute_single(line);
        }
    } else {
        cout << errorMsg << "Unable to open file" << defaultCor << "\n";
    }
}

/*
    Tach dong lenh thanh cac arguments
*/
vector<string> split_space(string inst) {
    vector<string> args;
    string s = "";
    inst += " ";
    for (int i = 0; i < sz(inst); ++ i) {
        if (inst[i] == ' ') {
            if (s != "") args.push_back(s), s = "";
        } else s += inst[i];
    }
    return args;
}

/*
    Doan nay da het bug :))
    Check ctrl-C co duoc press ko bang signal
*/

//void my_handler(sig_atomic_t s) {
//    TerminateProcess(fgProcess.pi.hProcess, 0);
//    CloseHandle(fgProcess.pi.hProcess);
//    CloseHandle(fgProcess.pi.hThread);
//    cout << "Process killed..." << "\n";
//}

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
        case CTRL_C_EVENT:
            cout << color[4] << "Ctrl-C pressed. Terminating process..." << defaultCor  << "\n";
            TerminateProcess(fgProcess.pi.hProcess, 0);
            CloseHandle(fgProcess.pi.hProcess);
            CloseHandle(fgProcess.pi.hThread);
            return TRUE;

        default:
            return FALSE;
    }
}

/*
    Chay file .exe va file .bat
*/
void _run(string inst) {

    if (strstr(inst.c_str(), ".bat") != NULL) {
        vector<string> args = split_space(inst);
        if (sz(args) != 2) {
            cout << errorMsg << "Invalid option" << defaultCor << "\n";
            cout << color[5] << "USAGE for .bat: run <batFile>" << defaultCor << "\n";
            return;
        }
        string fileName = args[1];
        runDotBat(fileName);
    } else if (strstr(inst.c_str(), ".exe") != NULL) {
        vector<string> args = split_space(inst);
        if (sz(args) != 3 || (args[1] != "-f" && args[1] != "-b")) {
            cout << errorMsg << "Invalid option" << defaultCor << "\n";
            cout << color[5] << "USAGE for .exe: run <OPTION> <exeFile>" << "\n";
            cout << "OPTION for .exe:" << "\n";
            cout << "  -f: foreground mode" << "\n";
            cout << "  -b: background mode" << defaultCor << "\n";
            return;
        }
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        string exeFile = args[2];
        CreateProcess(exeFile.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, (LPSTARTUPINFOA) &si, &pi);
        if (pi.dwProcessId == 0) {
            cout << "ERROR: File does not exist" << "\n";
            return;
        }

        if (args[1] == "-f") {
            fgProcess = {pi.dwProcessId, exeFile, 0, pi};
            // https://stackoverflow.com/a/1641190/23590989

            SetConsoleCtrlHandler(CtrlHandler, TRUE);
            WaitForSingleObject(pi.hProcess, INFINITE);
            TerminateProcess(pi.hProcess, 0);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else if (args[1] == "-b") {
            listProcess.push_back({pi.dwProcessId, exeFile, 0, pi});
        }
    } else {
        cout << errorMsg << "Shell only supports .bat and .exe file extension" << defaultCor << "\n";
    }
}

/*
    Dung mot tien trinh background
*/
void _stop(string inst) {
    vector<string> args = split_space(inst);
    if (sz(args) != 2) {
        cout << errorMsg << "Invalid option" << defaultCor << "\n";
        cout << usageMsg << "stop <PROCESS_ID>" << defaultCor << "\n";
        return;
    }
    unsigned int process_id = (unsigned int) atoi(args[1].c_str());
    for (auto &c : listProcess) {
        if (c.pi.dwProcessId == process_id) {
            if (c.status == 1) {
                cout << color[2] << "This process is currently stopping" << defaultCor << "\n";
                return;
            }
            c.status = 1;
            SuspendThread(c.pi.hThread);
            cout << color[2] << "Stop process " << process_id << " successfully" << defaultCor << "\n";
            return;
        }
    }
    cout << errorMsg << "Process " << process_id << " does not exist" << defaultCor << "\n";
}

/*
    Tiep tuc mot tien trinh background dang stop
*/
void _resume(string inst) {
    vector<string> args = split_space(inst);
    if (sz(args) != 2) {
        cout << errorMsg << "Invalid option" << defaultCor << "\n";
        cout << usageMsg << "resume <PROCESS_ID>" << defaultCor << "\n";
        return;
    }
    unsigned int process_id = (unsigned int) atoi(args[1].c_str());
    for (auto &c : listProcess) {
        if (c.pi.dwProcessId == process_id) {
            if (c.status == 0) {
                cout << color[2] << "This process is currently running" << defaultCor << "\n";
                return;
            }
            c.status = 0;
            ResumeThread(c.pi.hThread);
            cout << color[2] << "Resume process " << process_id << " successfully" << defaultCor << "\n";
            return;
        }
    }
    cout << errorMsg << "Process " << process_id << " does not exist" << defaultCor << "\n";
}

/*
    Dung vinh vien mot tien trinh background xoa khoi list
*/
void _kill(string inst) {
    vector<string> args = split_space(inst);
    if (sz(args) > 2) {
        cout << errorMsg << "Invalid option" << defaultCor << "\n";
        cout << usageMsg << "kill <PROCESS_ID>" << defaultCor << "\n";
        return;
    }
    unsigned int process_id = (unsigned int) atoi(args[1].c_str());
    for (int i = 0; i < sz(listProcess); ++ i) {
        process &c = listProcess[i];
        if (c.pi.dwProcessId == process_id) {
            TerminateProcess(c.pi.hProcess, 0);
            CloseHandle(c.pi.hProcess);
            CloseHandle(c.pi.hThread);
            cout << color[2] << "Kill process " << process_id << " successfully" << defaultCor << "\n";
            listProcess.erase(listProcess.begin()+i); // delete from vector
            return;
        }
    }
    cout << errorMsg << "Process " << process_id << " does not exist" << defaultCor << "\n";
}

/*
    lay file name tu mot cai path dai
    C:/abc/xyz/cde.exe -> cde.exe
*/

string getFileName(string dir) {
    if (dir[1] != ':') return dir;
    int cnt = 0;
    int len = sz(dir);
    for (int i = len - 1; i >= 0; -- i) {
        if (dir[i] == (TCHAR) '\\') {
            len = i;
            break;
        }
        cnt ++;
    }
    return dir.substr(len + 1, cnt);
}

/*
    Hien thi tat ca cac process dang chay ngam
*/
void _list() {
    cout << "\tID\t\t\tNAME\t\t\tSTATUS\n\n";
    for (int i = listProcess.size() - 1; i >= 0; -- i) {
        int processID = listProcess[i].id;
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processID);
        if (hProcess == NULL) {
            listProcess.erase(listProcess.begin()+i);
            continue;
        }
        DWORD exitCode;
        if (!GetExitCodeProcess(hProcess, &exitCode)) {
            listProcess.erase(listProcess.begin()+i);
            CloseHandle(hProcess);
            continue;
        }
        CloseHandle(hProcess);
        if (exitCode == STILL_ACTIVE) continue;
        listProcess.erase(listProcess.begin()+i);
    }
    for (auto c : listProcess) {
        cout << "      " << setw(10) << left << c.id;
        cout << setw(20) << right << getFileName(c.cmdName);
        cout << setw(27) << (c.status == 0 ? "running":"stopped");
        cout << "\n";
    }
}

// https://stackoverflow.com/questions/18719571/getting-system-env-in-c-from-windows
void _path() {
    char *path_str = getenv("PATH");
    if (path_str)
        cout << color[2] << "Value of PATH is: \n" + string(path_str) << defaultCor << "\n";
    else
        cout << errorMsg << "Empty or not defined" << defaultCor << "\n";
}

// Document: https://stackoverflow.com/questions/63534708/append-to-a-user-environment-variable-with-regsetvalueex-in-c
void _addpath(string inst) {
    vector<string> args = split_space(inst);
    if (sz(args) != 2) {
        cout << errorMsg << "Invalid option" << defaultCor << "\n";
        cout << usageMsg << "addpath <NEW_PATH>" << defaultCor << "\n";
        return;
    }
    // Open the registry key where the PATH environment variable is stored
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS) {
        cout << errorMsg << "Failed to open registry key." << defaultCor << "\n";
        return;
    }
    // Read the current PATH variable
    char currentPath[1024];
    DWORD bufferSize = sizeof(currentPath);
    if (RegQueryValueExA(hKey, "Path", NULL, NULL, (LPBYTE)currentPath, &bufferSize) != ERROR_SUCCESS) {
        cout << errorMsg << "ERROR: Failed to read current PATH." << defaultCor << endl;
        RegCloseKey(hKey);
        return;
    }
    string updatedPath = string(currentPath) + ";" + args[1];
    // Write the updated PATH back to the registry
    if (RegSetValueExA(hKey, "Path", 0, REG_EXPAND_SZ, (const BYTE*)updatedPath.c_str(), updatedPath.size() + 1) != ERROR_SUCCESS) {
        cerr << errorMsg << "Failed to update PATH." << defaultCor << "\n";
    } else {
        cout << color[2] << "PATH updated successfully." << defaultCor << "\n";
    }

    RegCloseKey(hKey);
    SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, NULL);
}

/*
    Ham lay thu muc cha
*/

string getPreviousPath(string dir) {
    int len = dir.size();
    for (int i = len - 1; i >= 0; -- i) {
        if (dir[i] == (TCHAR) '\\') {
            len = i;
            break;
        }
    }
    return dir.substr(0, len);
}

void _cd(string inst) {
    vector<string> args = split_space(inst);
    if (sz(args) != 2) {
        cout << errorMsg << "Invalid option" << defaultCor << "\n";
        cout << usageMsg << "cd <NEW_PATH/SUB_DIR>" << defaultCor << "\n";
        return;
    }
    string new_dir = args[1];
    if (new_dir == ".") {
        return;
    }
    if (new_dir == "..") {
        new_dir = getPreviousPath(currentDir);
        if (!SetCurrentDirectory(new_dir.c_str())) {
            cout << errorMsg << "Invalid directory" << defaultCor << "\n";
        } else {
            currentDir = new_dir;
        }
    } else if (new_dir[1] == ':') { // new dir
        if (!SetCurrentDirectory(new_dir.c_str())) {
            cout << errorMsg << "Invalid directory" << defaultCor << "\n";
        } else {
            currentDir = new_dir;
        }
    } else { // sub dir
        new_dir = currentDir + "\\" + new_dir;
        if (!SetCurrentDirectory(new_dir.c_str())) {
            cout << errorMsg << "Invalid directory" << defaultCor << "\n";
        } else {
            currentDir = new_dir;
        }
    }
}
