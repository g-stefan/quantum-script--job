@echo off
rem Public domain
rem http://unlicense.org/
rem Created by Grigore Stefan <g_stefan@yahoo.com>

set ACTION=%1
if "%1" == "" set ACTION=make

echo -^> %ACTION% quantum-script-extension-job

goto StepX
:cmdX
%*
if errorlevel 1 goto cmdXError
goto :eof
:cmdXError
echo "Error: %ACTION%"
exit 1
:StepX

call :cmdX file-to-cs --touch=source/quantum-script-extension-job.cpp --file-in=source/quantum-script-extension-job.js --file-out=source/quantum-script-extension-job.src --is-string --name=extensionJobSource
call :cmdX xyo-cc --mode=%ACTION% @util/quantum-script-extension-job.static.compile
call :cmdX xyo-cc --mode=%ACTION% @util/quantum-script-extension-job.dynamic.compile

