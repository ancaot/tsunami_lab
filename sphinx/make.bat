@ECHO OFF

pushd %~dp0

REM Command file for Sphinx documentation

if "%SPHINXBUILD%" == "" (
	set SPHINXBUILD=python -m sphinx
)
set SOURCEDIR=source
set BUILDDIR=build

%SPHINXBUILD% --version >NUL 2>NUL
if errorlevel 1 (
	echo.
	echo.The Sphinx command was not found. Make sure Python is installed,
	echo.then set the SPHINXBUILD environment variable to point to a working
	echo.Sphinx command. Alternatively you may add the Sphinx Scripts
	echo.directory to PATH.
	echo.
	echo.If you don't have Sphinx installed, grab it from
	echo.https://www.sphinx-doc.org/
	exit /b 1
)

if "%1" == "" goto help

%SPHINXBUILD% -M %1 %SOURCEDIR% %BUILDDIR% %SPHINXOPTS% %O%
goto end

:help
%SPHINXBUILD% -M help %SOURCEDIR% %BUILDDIR% %SPHINXOPTS% %O%

:end
popd
