# Microsoft Developer Studio Project File - Name="translator" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=translator - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "translator.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "translator.mak" CFG="translator - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "translator - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "translator - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "translator - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../bin"
# PROP Intermediate_Dir "../tmp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(SMIDIR)/src/commonSource" /I "$(SMIDIR)/smixx" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"../bin/smiTrans.exe"

!ELSEIF  "$(CFG)" == "translator - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\tmp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "$(SMIDIR)/src/commonSource" /I "$(SMIDIR)/smixx" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../bin/smiTrans.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "translator - Win32 Release"
# Name "translator - Win32 Debug"
# Begin Source File

SOURCE=..\src\translator\action.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\actionheadblock.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\attributeblock.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\attributes.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\block_name.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\boolitem.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\booloperation.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\condition.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\doins.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\getline_test.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\ifins.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\ifunit.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\ifunitheadblock.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\insertins.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\inslist.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\instruction.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\isofclassobject.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\main.cxx
# End Source File
# Begin Source File

SOURCE=..\src\commonSource\name.cxx
# End Source File
# Begin Source File

SOURCE=..\src\commonSource\namelist.cxx
# End Source File
# Begin Source File

SOURCE=..\src\commonSource\namevector.cxx
# End Source File
# Begin Source File

SOURCE=..\src\commonSource\nmdptnr.cxx
# End Source File
# Begin Source File

SOURCE=..\src\commonSource\nmdptnrvector.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\parameterblock.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\parameters.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\parms.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\processcommandline.cxx
# End Source File
# Begin Source File

SOURCE=..\src\commonSource\registrar.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\setins.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\smiobject.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\smiobjectset.cxx
# End Source File
# Begin Source File

SOURCE=..\src\commonSource\smixx_parstring_util.c
# End Source File
# Begin Source File

SOURCE=..\src\commonSource\smlline.cxx
# End Source File
# Begin Source File

SOURCE=..\src\commonSource\smllinevector.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\smlunit.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\smpcond.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\smpcondtyp1.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\smpcondtyp2.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\stack.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\stackitem.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\state.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\termins.cxx
# End Source File
# Begin Source File

SOURCE=..\src\commonSource\utilities.cxx
# End Source File
# Begin Source File

SOURCE=..\src\translator\when.cxx
# End Source File
# End Target
# End Project
