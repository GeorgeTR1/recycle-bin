@echo off

SET ASM=
REM SET ASM=/FAsu
SET UNICODE=
SET UNICODE=/DUNICODE /D_UNICODE
SET CFLAGS=/nologo /W4 /wd4996 /O2 /GS- /utf-8 %ASM% %UNICODE%
SET SUBS=console
REM SET SUBS=windows
SET SHRINK=
REM SET SHRINK=/merge:.rdata=. /merge:.pdata=. /merge:.text=. /section:.,ER /align:16 /ignore:4108,4254
SET LFLAGS=/fixed /incremental:no /opt:icf /opt:ref /subsystem:%SUBS% /entry:main %SHRINK%
REM SET LIBS=libvcruntime.lib ucrt.lib
SET LIBS=libvcruntime.lib

cl %CFLAGS% %1 %LIBS% /link %LFLAGS%
del *.obj