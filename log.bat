@echo off
if %1==/a goto OnApply
if %1==/u goto OnUndo
goto :Exit

:OnApply
md %2
cd %2
copy ..\log.vkp %3
goto :AddToLog

:OnUndo
cd %2
del %3

:AddToLog
echo.>> %2.log
echo.>> %2.log
echo _______________________________________________________________________________>> %2.log
date /t >> %2.log
time /t >> %2.log
if %1==/u goto AddToLogUndo
echo Apply patch: >> %2.log
goto :AddToLogNext
:AddToLogUndo
echo Undo patch: >> %2.log
:AddToLogNext
echo %3>> %2.log
echo %4>> %2.log
echo ------------------->> %2.log
copy /b %2.log + ..\log.vkp %2.log

:Exit
del ..\log.vkp

rem @echo off
rem echo %1
rem echo %2
rem echo %3
rem echo %4
rem pause
