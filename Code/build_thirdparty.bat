cd ThirdParty

mkdir Win32
mkdir x64

cd boost_1_59_0

if not exist b2.exe (
	call bootstrap.bat
)

set boost_components=--with-filesystem
set boost_common=%boost_components% toolset=msvc-14.0 link=static threading=multi runtime-link=shared

b2 %boost_common% stage
copy /Y stage\lib\*.lib ..\Win32\
rd /S /Q stage

b2 %boost_common% address-model=64 stage
copy /Y stage\lib\*.lib ..\x64\
rd /S /Q stage

cd..\..