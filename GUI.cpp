#include <iostream>
#include <windows.h>
#include <commdlg.h>
#include <cstdlib>
#include <thread>
#include "Writer.h"
#include "GUI.h"
#include <string>
#include <algorithm>


using namespace std;

string folderPath;
string fileName;
string disk;
string FileSys;
string FindUsbLet = "";
static string UsbLet = "";

#define btq 1
#define btWDid 2
#define edit_id 3
#define cb_id 4
#define btselIso 5
#define cb_id1 7
#define btSearchUSB_id 8

HWND edt = NULL;
HWND CB = NULL;
HWND CBfs = NULL;
HWND hwndPar = NULL;// id parent window
HWND log = NULL;

//powershell -NoProfile -Command "(Get-Partition -DriveLetter 'E').DiskNumber"
//this command need for get disk NUMBER

string SortFileN(string PaF) //PaN - path and file string
{
    int i = PaF.size() - 1;
    string Fn = ""; //file name

    while(i >= 0 && PaF[i] != '\\')
    {
        Fn += PaF[i];
        i--;
    }

    reverse(Fn.begin(), Fn.end());

    cout<<Fn<<endl;
    return Fn;
}

string SortFileP(string PaF) //PaN - path and file string
{
     if (PaF.empty()) return "";

    int i = PaF.size() - 1;

    while(i >= 0 && PaF[i] != '\\')
    {
        i--;
    }


    if (i >= 0) {
        string Fp = PaF.substr(0, i + 1);
        cout << Fp << endl;
        return Fp;
    }

    cout << "" << endl;
    return "";

}

void CrCBusb(string lett);

 void DropFile(HDROP hDrop)
 {
     char fName[MAX_PATH];
     int cnt = DragQueryFile(hDrop, 0xffffffff, NULL, 0);
     cout<<cnt<<endl;
     DragQueryFile(hDrop, 0, fName, MAX_PATH);
     cout<<fName<<endl;
     DragFinish(hDrop);
     fileName = SortFileN(fName);
     folderPath = SortFileP(fName);
     SetWindowText(edt, fileName.c_str());
     cout<<fileName<<endl;
     cout<<folderPath<<endl;

 }

void WinOpen()
{
    OPENFILENAME ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);

    ofn.lpstrFilter = "All Files\0*.*\0Text Files\0*.txt\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;

    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if(GetOpenFileName(&ofn) == TRUE)
    {
        char drive[10];
        char dir[260];
        char name[260];
        char ext[10];

        _splitpath(szFile, drive, dir, name, ext);

        folderPath = string(drive) + string(dir); //path
        fileName = string(name) + string(ext);

        disk = string(drive);
    }
}


DWORD WINAPI WriteDisk(LPVOID lpParam)
{
    format(UsbLet, FileSys, fileName);
    ISO_write(fileName, folderPath, UsbLet);

    return 0;
}

DWORD WINAPI SearchDisk(LPVOID lpParam)
{
    FindUsbLet = "";
    SendMessage(log, WM_SETTEXT, 0, (LPARAM)"Search disk");
    while(FindUsbLet == "")
    {
        int len = GetWindowTextLength(log);// get lenght text in log textbox
        SendMessage(log, EM_SETSEL, len, len);// set cursor after
        SendMessage(log, EM_REPLACESEL, FALSE, (LPARAM)".");// add text
        FindUsbLet = FindUsb();//check  value of FindUsbLet
    }
    CrCBusb(FindUsbLet);
    SendMessage(log, WM_SETTEXT, 0, (LPARAM)"Disk found!");

    return 0;
}

LRESULT WINAPI WndProc (HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    if(message == WM_DESTROY){
        string comToCmdUmount = string("powershell -command \"Dismount-DiskImage -ImagePath ") + "\"" + folderPath + fileName + "\"\"";
        system(comToCmdUmount.c_str());
        cout<<"OK: unmount image\n";
        PostQuitMessage(0);
    }

    if(LOWORD(wparam) == btselIso){
            WinOpen();
            SetWindowText(edt, fileName.c_str());

    }

    if(message == WM_DROPFILES)
        DropFile((HDROP)wparam);

    if(message == WM_COMMAND){


          if(LOWORD(wparam) == cb_id){

            if(HIWORD(wparam) == CBN_SELCHANGE){
                int id = SendMessage((HWND)lparam, CB_GETCURSEL, 0, 0);//get id
                char c[1];
                SendMessage((HWND)lparam, CB_GETLBTEXT, id, (LPARAM)c);//get string
                cout<<"selected"<<id<<endl; //view id
                cout<<c<<endl; // view string
                UsbLet = c[0];
            }
                return 0;
          }

          if(LOWORD(wparam) == cb_id1){
               // FindUsbLet = FindUsb();
            if(HIWORD(wparam) == CBN_SELCHANGE){
                int id = SendMessage((HWND)lparam, CB_GETCURSEL, 0, 0);//get id
                char c[4];
                SendMessage((HWND)lparam, CB_GETLBTEXT, id, (LPARAM)c);//get string
                cout<<"selected"<<id<<endl; //view id
                cout<<c<<endl; // view string
                FileSys = c;
                cout<<FileSys<<endl;
            }

                return 0;
          }

        if(LOWORD(wparam) == btq)
            PostQuitMessage(0);

        if(LOWORD(wparam) == btSearchUSB_id){
            HANDLE hThreadSD = CreateThread(NULL, 0, SearchDisk, NULL, 0, NULL);  // create thread for search disk button
            if(hThreadSD == NULL){
                CloseHandle(hThreadSD);
            }
        }

        if(LOWORD(wparam) == btWDid)
        {
            HANDLE hTheadWD = CreateThread(NULL, 0, WriteDisk, NULL, 0, NULL); // create thread for write disk button
            if(hTheadWD == NULL){
                CloseHandle(hTheadWD);
            }
        }



        if(LOWORD(wparam) == edit_id)
        {
            int nc = HIWORD(wparam);
            char c[100];
            GetWindowText((HWND)lparam, c, 100);
            if(nc == EN_UPDATE)
                cout<<"   EN_UPDATE " << c << endl;
            if(nc == EN_CHANGE)
                cout<<"   EN_CHANGE " << endl;
        }



    }

     else return DefWindowProcA(hwnd, message, wparam, lparam);
}

void CrCBusb(string lett){
    //add usb let in combobox
        for(int ii = 0;ii<lett.length();ii++){
            if(lett[ii] != '\n'){
               string StrForCB = string(1,lett[ii]) + ":"; //create string for combobox, because only one symbol WinAPI does n`t accept
               cout<< "hexStrCB: " << hex <<(int)FindUsbLet[ii]<<endl;
               SendMessage(CB, CB_ADDSTRING, 0, (LPARAM)StrForCB.c_str());
               cout<< "StrCB: " <<StrForCB<<endl;
            }
        }
}


void CrWindow()
{
    {
    WNDCLASSA wcl;
        memset(&wcl, 0, sizeof(WNDCLASSA));
        wcl.lpszClassName = "WinISOWrite";
        wcl.lpfnWndProc = WndProc;
    RegisterClassA(&wcl);


    hwndPar = CreateWindowA("WinISOWrite", "WinISOWrite",  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                        10,10, 390,380, NULL, NULL, NULL, NULL);

    ShowWindow(hwndPar, SW_SHOWNORMAL);
    DragAcceptFiles(hwndPar, TRUE);

    HWND bt_quit;
    bt_quit = CreateWindow("button", "Quit",
                           WS_VISIBLE | WS_CHILD,
                           145,110, 100,50,
                           hwndPar, (HMENU)btq, NULL, NULL);

    HWND btOpenFile;
    btOpenFile = CreateWindow("button", "O",
                           WS_VISIBLE | WS_CHILD,
                           350,29, 24,24,
                           hwndPar, (HMENU)btselIso, NULL, NULL);

    HWND btWD;
    btWD = CreateWindow("button", "Write disk",
                           WS_VISIBLE | WS_CHILD,
                           10,110, 100,50,
                           hwndPar, (HMENU)btWDid, NULL, NULL);

    HWND btSearchUSB;
    btSearchUSB = CreateWindow("button", "Search disk",
                           WS_VISIBLE | WS_CHILD,
                           272,110, 100,50,
                           hwndPar, (HMENU)btSearchUSB_id, NULL, NULL);

    edt = CreateWindow("edit", NULL, //fileName.c_str()
                            WS_VISIBLE | WS_CHILD | WS_BORDER,
                            150,30, 200,23,
                            hwndPar, (HMENU)edit_id, NULL, NULL);

    log = CreateWindow("edit", "",
                            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
                            10,170, 365,150,
                            hwndPar, (HMENU)logs, NULL, NULL);

    HWND textSelUSB = CreateWindow("static", "Select a USB drive",
                            WS_VISIBLE | WS_CHILD,
                            10,10, 85,15,
                            hwndPar, (HMENU)edit_id, NULL, NULL);

    HWND textSelISO = CreateWindow("static", "Select a ISO file",
                            WS_VISIBLE | WS_CHILD,
                            150,10, 80,15,
                            hwndPar, (HMENU)edit_id, NULL, NULL);

    HWND textSelFs = CreateWindow("static", "Select a file system",
                            WS_VISIBLE | WS_CHILD,
                            10,60, 130,16,
                            hwndPar, (HMENU)edit_id, NULL, NULL);

        CBfs = CreateWindow("combobox", "",
                           WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                           10,80, 100,190,
                           hwndPar, (HMENU)cb_id1, NULL, NULL);

        SendMessage(CBfs, CB_ADDSTRING, 0, (LPARAM) "NTFS");
        SendMessage(CBfs, CB_ADDSTRING, 0, (LPARAM) "FAT32");

        CB = CreateWindow("combobox", "",
                           WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                           10,30, 100,150,
                           hwndPar, (HMENU)cb_id, NULL, NULL);


    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        DispatchMessage(&msg);
    }

}


}
//ReadCmd()
