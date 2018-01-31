@rem Convenience script for building and running test suite.

@echo OFF
@setlocal EnableDelayedExpansion

call %~dp0\_build\library.bat

if %ERRORLEVEL% equ 0 (
  echo Succeeded.
) else (
  echo Failed!
  exit /B 1
)
