#include <windows.h>

#pragma comment (lib, "Shell32")
#pragma comment (lib, "Kernel32")
#pragma comment (lib, "User32")

void fputsWin(char *str, DWORD stdHandle) {
   HANDLE h = GetStdHandle(stdHandle);
   if (h == NULL || h == INVALID_HANDLE_VALUE) ExitProcess(1);
   
   {
      BOOL ret = WriteFile(h, str, lstrlenA(str), NULL, NULL);
      if (!ret) ExitProcess(1);
   }
}

int mainNoCRT() {
   int argc;
   HANDLE hHeap;
   DWORD *lens;
   size_t len = 1; // include room for double null termination
   int i;
   wchar_t *from;
   wchar_t *pos;
   
   wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
   if (argv == NULL) ExitProcess(1);
   
   if (argc == 2) {
      if (lstrcmpW(argv[1], L"--version") == 0) {
         fputsWin("1.2.0\n", STD_OUTPUT_HANDLE);
         ExitProcess(0);
      }

      if (lstrcmpW(argv[1], L"--help") == 0) {
         // usage description follows standard described at http://docopt.org/
         fputsWin("\n  Move files and folders to the recycle bin\n\n"
         "  Usage: recycle (--version | --help | <path>...)\n", STD_OUTPUT_HANDLE);
         ExitProcess(0);
      }
   }
   
   /* There is a quirk with SHFileOperation on old versions of Windows where
      full path names have to specified or deleting to the recycle bin won't
      work. See 
      https://web.archive.org/web/20231128043232/http://www.maddogsw.com/cmdutils/cmdutils.zip */
   
   if (argc == 1) {
      fputsWin("Specify at least one path\n", STD_ERROR_HANDLE);
      ExitProcess(1);
   }
   
   hHeap = GetProcessHeap();
   if (hHeap == NULL) ExitProcess(1);
   
   lens = HeapAlloc(hHeap, 0, argc * sizeof(DWORD));
   if (lens == NULL) ExitProcess(1);

   for (i = 1; i < argc; i++) {
      // returns the length needed for the full path name, including terminating null
      DWORD ret = GetFullPathNameW(argv[i], 0, NULL, NULL);
      if (ret == 0) ExitProcess(1);
      lens[i] = ret;
      len += ret;
   }
   
   from = HeapAlloc(hHeap, 0, len  * sizeof(wchar_t));
   if (from == NULL) ExitProcess(1);
   pos = from;

   for (i = 1; i < argc; i++) {
      /* GetFullPathName fills the entire unused length of the buffer given, which
         is provided in the second parameter, with zeros. My guess is this is
         because it uses strncpy internally. By specifying the proper length from
         the "lens" array for each call we avoid unnecessary filling of the buffer
         with zeros */
      DWORD ret = GetFullPathNameW(argv[i], lens[i], pos, NULL);
      if (ret == 0) ExitProcess(1);
      pos += ret + 1; // add 1 for terminating null
   }
   *pos = L'\0';
   
   {
      SHFILEOPSTRUCTW op = {0};
      int ret;

      op.wFunc = FO_DELETE;
      op.pFrom = from;
      op.fFlags = FOF_ALLOWUNDO | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;

      ret = SHFileOperationW(&op);
   
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
         case 0x402:
            fputsWin("Invalid path\n", STD_ERROR_HANDLE);
            break;
         default:
            {
               char buf[50];
               wsprintfA(buf, "Unknown error: 0x%X\n", ret);
               fputsWin(buf, STD_ERROR_HANDLE);
               break;
            }
      }

      /* apparently there can be race conditions when returning from an entry
         point due to misbehaved DLLs, so it's safer to use ExitProcess instead of
         returning directly */
      ExitProcess(ret);
   }
}
