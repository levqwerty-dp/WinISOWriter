#include <iostream>
#include <windows.h>
#include <string>
#include <commdlg.h>
#include <shobjidl.h>
#include <memory>
#include <array>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "Writer.h"
#include "GUI.h"
#include <thread>

using namespace std;

void WinOpen();



string ReadCmd(string cmd)
{
    char buffer[256];
    string result;

    FILE* pipe = _popen(cmd.c_str(), "r");

    if(!pipe){
        throw runtime_error("_popen() failed!");
    }
    //read cmd
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    while (!result.empty() && (result.back() == '\n' || result.back() == '\r' || result.back() == ' ')) {
        result.pop_back();
    }

    _pclose(pipe);
    cout<<"result: "<< result<<endl;
    return result;

}


void format(string disk, string fs, string NamePart)
{
    string comTocmd = "echo. | format " + disk +": /FS:" + fs + " /V:" + NamePart + " /Q /Y"; // command to
    system(comTocmd.c_str());
}

//find letter for usb
string FindUsb(){
    string findUSBflash = "powershell -Command \"(Get-Disk | Where-Object {$_.BusType -eq 'USB'} | Get-Partition).DriveLetter\"";
    string UsbLet = ReadCmd(findUSBflash);
    cout<<"usb:"<<UsbLet <<endl;
    return UsbLet;
}


void ISO_write(string iso, string pathISO, string UsbLet)
{
     if (UsbLet.empty()) { // checking usb drive
        std::cout << "flash not found!" << std::endl;
        std::cout << "NO: " << UsbLet << ":" << std::endl;
    } else {
        std::cout << "OK: " << UsbLet << ":" << std::endl;

        //find iso image letter
        string findISOdisk = "powershell -Command \"(Mount-DiskImage -ImagePath '\"" + pathISO + iso + "\"' -PassThru | Get-Volume).DriveLetter\"";
        string DiskLett = ReadCmd(findISOdisk);

        string comToCmd1 = "cd /d " + pathISO + " &&  \"" + iso + "\"";
        cout <<"59:Writer:"+DiskLett<<endl;
        string comToCmd2 = "xcopy \"" + DiskLett + ":\\*.*\" \"" + UsbLet + ":\\\" /E /Y /H /R"; //write
        string comToCmdUmount = string("powershell -command \"Dismount-DiskImage -ImagePath ") + "\"" + pathISO + iso + "\"\"";

        //string process_str = ReadCmd(comToCmd2);

        system(comToCmd1.c_str());

        char buffer[256];
        string output = "";

        FILE* pipe = _popen(comToCmd2.c_str(), "r");
        if (pipe) {


            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                OemToCharA(buffer, buffer);
                output += buffer;
                output += "\r\n";
                SetDlgItemTextA(hwndPar, 6, output.c_str());
                cout << buffer<<endl;

                HWND hLogCtrl = GetDlgItem(hwndPar, 6);
                if (hLogCtrl != NULL) {
                    int lineCount = SendMessage(hLogCtrl, EM_GETLINECOUNT, 0, 0);
                    SendMessage(hLogCtrl, EM_LINESCROLL, 0, lineCount);
                }
            }
            _pclose(pipe);
        } else {
            cout << "error" << endl;
        }



        cout<<"OK: open and write disk\n";
        system(comToCmdUmount.c_str());
        cout<<"OK: unmount image\n";

    }
}




//disk

