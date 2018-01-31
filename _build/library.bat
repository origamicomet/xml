@rem Script to build for Windows using Visual Studio.

@echo OFF
@setlocal EnableDelayedExpansion

set ROOT=%~dp0\..

@rem Detect if the build has been initiated by Visual Studio.
if defined VisualStudioVersion (
  set IDE=1
) else (
  set IDE=0
)

if not defined TOOLCHAIN (
  if not defined VisualStudioVersion (
    @rem Use whatever toolchain we can.
    echo Using latest Visual Studio install... 1>&2
    set TOOLCHAIN=latest
  ) else (
    @rem Use toolchain corresponding to Visual Studio that initiated the build.
    set TOOLCHAIN=%VisualStudioVersion%
  )
)

@rem Default to debug builds.
if not defined DEBUG (set DEBUG=1)

@rem Default to Windows builds.
if not defined TARGET (set TARGET=windows)

@rem Default to x86_64.
if not defined ARCHITECTURE (set ARCHITECTURE=x86_64)

@rem Setup environment for build.
call %ROOT%\_build\scripts\vc.bat %TOOLCHAIN% %TARGET% %ARCHITECTURE%

if not %ERRORLEVEL% equ 0 (
  echo Could not setup environment to target %TARGET% and/or %ARCHITECTURE%!
  exit /B 1
)

if %ARCHITECTURE% equ x86 (
  set CFLAGS=!CFLAGS! /arch:IA32 /fp:except /favor:blend
  set ARFLAGS=!ARFLAGS! /machine:X86
)

if %ARCHITECTURE% equ x86_64 (
  set CFLAGS=!CFLAGS! /fp:except /favor:blend
  set ARFLAGS=!ARFLAGS! /machine:X64
)

if %DEBUG% equ 1 (
  set CFLAGS=!CFLAGS! /Od /Oi
  set CFLAGS=!CFLAGS! /Zi /Fd%ROOT%\_build\xml.pdb
  set CFLAGS=!CFLAGS! /Gm- /GR- /EHa- /GS
) else (
  set CFLAGS=!CFLAGS! /O1 /Oi
  set CFLAGS=!CFLAGS! /Gm- /GR- /EHa- /GS- /Gs134217728
)

cl.exe /nologo /c /W4 %CFLAGS% ^
       /I%ROOT% /I%ROOT%\xml ^
       /Fo%ROOT%\_build\xml.obj ^
       /Tp%ROOT%\xml.c

if not %ERRORLEVEL% equ 0 (
  echo Compilation failed.
  exit /B 1
)

lib.exe /nologo %ARFLAGS% ^
        /out:%ROOT%\_build\xml.lib ^
        %ROOT%\_build\xml.obj

if not %ERRORLEVEL% equ 0 (
  popd
  echo Archiving failed.
  exit /B 1
)

echo Built `xml.lib`.
