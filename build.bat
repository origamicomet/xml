@rem Convenience script for building and running test suite.

@echo OFF
@setlocal EnableDelayedExpansion

call %~dp0\_build\library.bat

if not %ERRORLEVEL% equ 0 (
  echo Failed!
  exit /B 1
)

call %~dp0\_build\example.bat

if not %ERRORLEVEL% equ 0 (
  echo Failed!
  exit /B 1
)

echo Succeeded.
