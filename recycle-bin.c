#include <windows.h>

#pragma comment (lib, "Shell32")
#pragma comment (lib, "Kernel32")
#pragma comment (lib, "User32")

void fputsWin(char *str, DWORD stdHandle) {
   HANDLE h = GetStdHandle(stdHandle);
   if (h == NULL || h == INVALID_HANDLE_VALUE) ExitProcess(1);
   
   BOOL ret = WriteFile(h, str, lstrlenA(str), NULL, NULL);
   if (!ret) ExitProcess(1);
}

int main() {
   int argc;
   wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
   if (argv == NULL) ExitProcess(1);
   
   if (argc == 2) {
      if (lstrcmpW(argv[1], L"--version") == 0) {
         fputsWin("1.1.0\n", STD_OUTPUT_HANDLE);
         ExitProcess(0);
      }

      if (lstrcmpW(argv[1], L"--help") == 0) {
         fputsWin("\n  Move files and folders to the recycle bin\n\n"
         "  Usage: recycle <path> [...]\n", STD_OUTPUT_HANDLE);
         ExitProcess(0);
      }
   }

   if (argc == 1) {
      fputsWin("Specify at least one path\n", STD_ERROR_HANDLE);
      ExitProcess(1);
   }

   size_t len = argc;

   for (int i = 1; i < argc; i++) {
      len += lstrlenW(argv[i]);
   }
   
   HANDLE hHeap = GetProcessHeap();
   if (hHeap == NULL) ExitProcess(1);
   
   wchar_t *from = HeapAlloc(hHeap, 0, len * sizeof(wchar_t));
   if (from == NULL) ExitProcess(1);

   size_t pos = 0;

   for (int i = 1; i < argc; i++) {
      wchar_t *ret = lstrcpyW(&from[pos], argv[i]);
      if (ret == NULL) ExitProcess(1);
      pos += lstrlenW(argv[i]) + 1;
   }

   from[pos] = '\0';

   SHFILEOPSTRUCTW op = {0};

   op.wFunc = FO_DELETE;
   op.pFrom = from;
   op.fFlags = FOF_ALLOWUNDO | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;

   int ret = SHFileOperationW(&op);
   
   switch (ret) {
      case 0:
         break;
      case 0x7C:
      case 0x2:
      case 0x3:
         fputsWin("File not found\n", STD_ERROR_HANDLE);
         break;
      case 0x78:
      case 0x5:
      case 0x20:
      case 0x21:
         fputsWin("Access denied\n", STD_ERROR_HANDLE);
         break;
      case 0x79:
      case 0x81:
      case 0xB7:
         fputsWin("File path too long\n", STD_ERROR_HANDLE);
         break;
      default:
         char buf[50];
         wsprintfA(buf, "Unknown error: 0x%X\n", ret);
         fputsWin(buf, STD_ERROR_HANDLE);
         break;
   }

   ExitProcess(ret);
}
