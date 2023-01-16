@rem **************************************************************************
@rem WWIVdbg Build Script.
@rem
@rem Required Variables:
@rem WORKSPACE - git root directory
@rem BUILD_NUMBER - Jenkins Build number
@rem
@rem Installed Software:
@rem   7-Zip [C:\Program Files\7-Zip\7z.exe]
@rem   Visual Studio [C:\Program Files (x86)\Microsoft Visual Studio\VER]
@rem   cmake [in PATH, set by vcvarsall.bat]
@rem 
@rem **************************************************************************

setlocal
@echo off

del wwiv-*.zip

if /I "%LABEL%"=="win-x86" (
	@echo "Setting x86 (32-bit) Architecture"
	set WWIVDBG_ARCH=x86
)
if /I "%LABEL%"=="win-x64" (
	@echo "Setting x64 (64-bit) Architecture"
	set WWIVDBG_ARCH=x64
)
set WWIVDBG_DISTRO=%LABEL%

set ZIP_EXE="C:\Program Files\7-Zip\7z.exe"
set WWIVDBG_RELEASE=1.0.0
set WWIVDBG_FULL_RELEASE=%WWIVDBG_RELEASE%.%BUILD_NUMBER%
set WWIVDBG_RELEASE_ARCHIVE_FILE=wwiv-%WWIVDBG_DISTRO%-%WWIVDBG_FULL_RELEASE%.zip
set CMAKE_BINARY_DIR=%WORKSPACE%\_build
set WWIVDBG_RELEASE_DIR=%CMAKE_BINARY_DIR%\release

@rem ===============================================================================

call %VCVARS_ALL% %WWIVDBG_ARCH%

@echo =============================================================================
@echo Workspace:            %WORKSPACE% 
@echo Label:                %LABEL%
@echo WWIVDBG_ARCHitecture:    %WWIVDBG_ARCH%
@echo WWIVDBG_DISTRO:          %WWIVDBG_DISTRO%
@echo WWIV Release:         %WWIVDBG_RELEASE%        
@echo Build Number:         %BUILD_NUMBER%
@echo WWIV CMake Root:      %CMAKE_BINARY_DIR%
@echo WWIVDBG_ARCHive:         %WWIVDBG_RELEASE_ARCHIVE_FILE%
@echo Release Dir:          %WWIVDBG_RELEASE_DIR%
@echo Visual Studio Shell:  %VCVARS_ALL%
@echo WindowsSdkVerBinPath  %WindowsSdkVerBinPath%
@echo WindowsLibPath        %WindowsLibPath%
@echo INCLUDE               %INCLUDE%
@echo =============================================================================

mkdir %CMAKE_BINARY_DIR%
del %CMAKE_BINARY_DIR%\CMakeCache.txt
rmdir /s/q %CMAKE_BINARY_DIR%\CMakeFiles
rmdir /s/q %CMAKE_BINARY_DIR%\Testing

cd %WORKSPACE%
mkdir %WWIVDBG_RELEASE_DIR%
del /q %WWIVDBG_RELEASE_DIR%
del /q wwivdbg-*.zip
del /q wwivdbg-*.exe

echo "Updating vcpkg"
call .\vcpkg\bootstrap-vcpkg.bat


echo * Building WWIV
cd %CMAKE_BINARY_DIR%
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ^
    -DWWIVDBG_RELEASE=%WWIVDBG_RELEASE% ^
    -DWWIVDBG_ARCH=%WWIVDBG_ARCH%  ^
    -DWWIVDBG_BUILD_NUMBER=%BUILD_NUMBER% ^
    -DWWIVDBG_DISTRO=%WWIVDBG_DISTRO% ^
    %WORKSPACE% || exit /b

cmake --build . --config Release || exit /b

@echo =============================================================================
@echo                           **** RUNNING TESTS ****
@echo =============================================================================
rem ctest --no-compress-output --output-on-failure -T Test 

echo * Creating release Archive: %WWIVDBG_RELEASE_ARCHIVE_FILE%
cpack -G ZIP || exit /b 

cd %WORKSPACE%
copy /y/v %CMAKE_BINARY_DIR%\%WWIVDBG_RELEASE_ARCHIVE_FILE% %WORKSPACE%\%WWIVDBG_RELEASE_ARCHIVE_FILE%

echo **** SUCCESS ****
echo ** Archive File: %WORKSPACE%\%WWIVDBG_RELEASE_ARCHIVE_FILE%
echo ** Archive contents:
%ZIP_EXE% l %WORKSPACE%\%WWIVDBG_RELEASE_ARCHIVE_FILE%
endlocal
