@echo off
REM Run this file to build the project outside of the IDE.
REM WARNING: if using a different machine, copy the .rsp files together with this script.
echo ZamCalcul.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamCalcul.gcc.rsp" || exit 1
echo ZamDCTE.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamDCTE.gcc.rsp" || exit 1
echo ZamDINV.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamDINV.gcc.rsp" || exit 1
echo ZamDPMC.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamDPMC.gcc.rsp" || exit 1
echo ZamDRTF.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamDRTF.gcc.rsp" || exit 1
echo ZamDVWC.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamDVWC.gcc.rsp" || exit 1
echo ZamDWMX.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamDWMX.gcc.rsp" || exit 1
echo Zamevse.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/Zamevse.gcc.rsp" || exit 1
echo ZamReportCC.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamReportCC.gcc.rsp" || exit 1
echo ZamConVE.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamConVE.gcc.rsp" || exit 1
echo ZamAddress.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamAddress.gcc.rsp" || exit 1
echo ZamBay.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamBay.gcc.rsp" || exit 1
echo ZamConAP.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamConAP.gcc.rsp" || exit 1
echo ZamCondeq.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamCondeq.gcc.rsp" || exit 1
echo ZamCondnode.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamCondnode.gcc.rsp" || exit 1
echo ZamDECP.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamDECP.gcc.rsp" || exit 1
echo ZamDEEVP.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamDEEVP.gcc.rsp" || exit 1
echo ZamDSTO.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamDSTO.gcc.rsp" || exit 1
echo ZamEqf.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamEqf.gcc.rsp" || exit 1
echo ZamEquip.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamEquip.gcc.rsp" || exit 1
echo ZamFSCHP.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamFSCHP.gcc.rsp" || exit 1
echo ZamFunc.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamFunc.gcc.rsp" || exit 1
echo ZamLDCS.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamLDCS.gcc.rsp" || exit 1
echo ZamLDSE.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamLDSE.gcc.rsp" || exit 1
echo ZamLLN0.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamLLN0.gcc.rsp" || exit 1
echo ZamLNode.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamLNode.gcc.rsp" || exit 1
echo ZamLPHD.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamLPHD.gcc.rsp" || exit 1
echo ZamMMXU.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamMMXU.gcc.rsp" || exit 1
echo ZamProcess.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamProcess.gcc.rsp" || exit 1
echo ZamReport.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamReport.gcc.rsp" || exit 1
echo ZamS118.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamS118.gcc.rsp" || exit 1
echo ZamServeur.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamServeur.gcc.rsp" || exit 1
echo ZamSignal.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamSignal.gcc.rsp" || exit 1
echo ZamStation.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamStation.gcc.rsp" || exit 1
echo ZamStemps.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamStemps.gcc.rsp" || exit 1
echo ZamSubnet.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamSubnet.gcc.rsp" || exit 1
echo ZamSubstation.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamSubstation.gcc.rsp" || exit 1
echo ZamTerm.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamTerm.gcc.rsp" || exit 1
echo ZamTypad.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamTypad.gcc.rsp" || exit 1
echo ZamUnit.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamUnit.gcc.rsp" || exit 1
echo ZamVL.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamVL.gcc.rsp" || exit 1
echo ZamXSWI.cpp
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @"VisualGDB/Debug/ZamXSWI.gcc.rsp" || exit 1
echo Linking ../VisualGDB/Debug/ZamStation...
E:\SysGCC\buster\bin\arm-linux-gnueabihf-g++.exe @../VisualGDB/Debug/ZamStation.link.rsp || exit 1
