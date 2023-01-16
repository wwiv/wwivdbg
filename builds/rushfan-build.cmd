SET WORKSPACE=W:\wwivdbg
SET BUILD_NUMBER=2112
set label=win-x86
set VCVARS_ALL="C:\Program Files\Microsoft Visual Studio\2022\Preview\VC\Auxiliary\Build\vcvarsall.bat"
pushd %WORKSPACE%
call %WORKSPACE%\builds\build.cmd || echo "Build FAILED!"
popd

