@echo off

IF [%1] == [] (
  echo Expecting a file name to compile
  exit /b 1
)

SET ASM=
REM SET ASM=/FAsu
SET UNICODE=
SET UNICODE=/DUNICODE /D_UNICODE
SET CFLAGS=/nologo /W4 /wd4996 /O2 /GS- /utf-8 %ASM% %UNICODE%
SET SUBS=console
REM SET SUBS=windows
SET LFLAGS=/fixed /incremental:no /opt:icf /opt:ref /subsystem:%SUBS% /entry:mainNoCRT
SET LIBS=libvcruntime.lib

cl %CFLAGS% %1 %LIBS% /link %LFLAGS%
del *.obj