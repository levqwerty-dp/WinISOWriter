#ifndef WRITER_H_INCLUDED
#define WRITER_H_INCLUDED

#include <string>

using namespace std;

enum FileSys { FAT32, NTFS};

extern string fileName;
extern string folderPath;

void format(string disk, string fs, string NamePart);
void ISO_write(string iso, string pathISO, string UsbLet);
string ReadCmd(string cmd);
string FindUsb();
void start();


#endif // WRITER_H_INCLUDED
