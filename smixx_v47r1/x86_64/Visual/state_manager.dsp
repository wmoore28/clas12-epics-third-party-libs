# Microsoft Developer Studio Project File - Name="state_manager" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=state_manager - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "state_manager.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "state_manager.mak" CFG="state_manager - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "state_manager - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "state_manager - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "state_manager - Win32 Release"

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
# ADD BASE F90 /include:"Release/"
# ADD F90 /include:"Release/"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "$(SMIDIR)/src/commonSource" /I "$(SMIDIR)/smixx" /I "$(DIMDIR)/dim" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib $(DIMDIR)/bin/dim.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"../bin/smiSM.exe"

!ELSEIF  "$(CFG)" == "state_manager - Win32 Debug"

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
# ADD BASE F90 /include:"Debug/"
# ADD F90 /include:"exe/"
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "$(SMIDIR)/src/commonSource" /I "$(SMIDIR)/smixx" /I "$(DIMDIR)/dim" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib $(DIMDIR)/bin/dim.lib /nologo /subsystem:console /profile /debug /machine:I386 /out:"../bin/smiSM.exe"

!ENDIF 

# Begin Target

# Name "state_manager - Win32 Release"
# Name "state_manager - Win32 Debug"
# Begin Source File

SOURCE=..\src\stateManager\action.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\block_name.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\clientobject.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\clientstate.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\clientwhens.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\commhandler.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\condition.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\doins.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\ifhandler.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\ifins.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\initiator.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\insertins.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\inslist.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\logic_engine.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\msg.cxx
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

SOURCE=..\src\commonSource\nmdptnrlist.cxx
# End Source File
# Begin Source File

SOURCE=..\src\commonSource\nmdptnrvector.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\objectregistrar.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\parms.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\processcommandline.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\ptrvector.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\queue_name.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\queue_twonames.cxx
# End Source File
# Begin Source File

SOURCE=..\src\commonSource\registrar.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\scheduler.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\set_name.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\setins.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\smiclass.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\smiobject.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\smiobjectset.cxx
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

SOURCE=..\src\stateManager\smpcond.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\smpcondtyp1.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\smpcondtyp2.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\state.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\state_manager.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\termins.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\twonames.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\ut_sm.cxx
# End Source File
# Begin Source File

SOURCE=..\src\commonSource\utilities.cxx
# End Source File
# Begin Source File

SOURCE=..\src\stateManager\when.cxx
# End Source File
# End Target
# End Project
