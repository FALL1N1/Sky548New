REM : Created by Goatform
@ECHO OFF
Title= .:: FTP Restarter ::.
CLS 

ECHO Core Started and Updated %time:~0,5% %date:~1%

:SERVERLOOP
worldserver.exe
xcopy /Y Update\*.*
ECHO Core Restarted and Updated %time:~0,5% %date:~1% 
ECHO.
GOTO SERVERLOOP
:END