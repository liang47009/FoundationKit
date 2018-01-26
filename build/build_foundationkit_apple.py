#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os
import time
import shutil #for file rm and copy
import fnmatch
import xcopy

#******************需要手动填写***********************
#xcode.app路径
xcodePath="/Applications/Xcode.app"
#工作空间
workspaceName="../FoundationKitFramework.xcworkspace"
#***************************************************

os.system("sudo xcode-select -switch "+xcodePath)
#获取当前目录路径
workspaceDirPath=os.getcwd()
buildPath = workspaceDirPath+"/build"

#shutil.rmtree(buildPath)
# schemeName
# configuration [Debug|Release] 使用xcodebuild -list查看configuration支持的类型
def Build(schemeName, configuration, rebuild):
    #BaseCommand
    baseCommand=" -workspace "+workspaceName+" -scheme "+schemeName+" -configuration "+configuration
    #清除编译的app
    xcodebuild_clean="xcodebuild clean"+baseCommand
    #编译
    xcodebuild_build="xcodebuild build"+baseCommand+" -derivedDataPath "+buildPath
    #查看全部的SDK
    xcodebuild_showsdks="xcodebuild -showsdks"
    if(rebuild == True):
        os.system(xcodebuild_clean)
    os.system(xcodebuild_build)


headerfilters=["*.hpp", "*.h", "*.inl"]
libfilters=["*.a"]

Build("FoundationKit_iOS", "Debug", True)
Build("FoundationKit_iOS", "Release", True)
xcopy.xcopy(workspaceDirPath+"/../FoundationKit/FoundationKit/", workspaceDirPath+"/../publish/iOS/include/FoundationKit/", headerfilters)
xcopy.xcopy(buildPath+"/Build/Products/", workspaceDirPath+"/../publish/iOS/libs/", libfilters)
shutil.rmtree(buildPath)

Build("FoundationKit_Mac", "Debug", True)
Build("FoundationKit_Mac", "Release", True)
xcopy.xcopy(workspaceDirPath+"/../FoundationKit/FoundationKit/", workspaceDirPath+"/../publish/Mac/include/FoundationKit/", headerfilters)
xcopy.xcopy(buildPath+"/Build/Products/", workspaceDirPath+"/../publish/Mac/libs/", libfilters)
shutil.rmtree(buildPath)




'''
#获取当前时间并格式化
dateTime=time.strftime("%Y-%m-%d-%H-%M-%S", time.localtime());

#获取当前目录路径
workspaceDirPath=os.getcwd()
#获取当前桌面路径
desktopPath=os.path.join(os.path.expanduser("~"), 'Desktop')
desktopPath2=os.path.expanduser("~/Desktop")

#最后生成基础目录
basePath=desktopPath+"/App"+dateTime
#archivePath目录
archivePath=basePath+"/archive"
if os.path.exists(archivePath)==False:
    os.makedirs(archivePath)
#exportPath目录
exportPath=basePath+"/ipa"
if os.path.exists(exportPath)==False:
    os.makedirs(exportPath)

for schemeName in schemeNames:
    #print(schemeName)

    #编译版本信息
    #sdkName= "iphoneos" #真机iphoneos 模拟器iphonesimulator
    configurationName="Debug" #debug版本[Debug|Release]，使用xcodebuild -list查看configuration支持的类型

    #BaseCommand
    #baseCommand=" -workspace "+workspaceName+" -scheme "+schemeName+" -configuration "+configurationName+" -sdk "+sdkName
    baseCommand=" -workspace "+workspaceName+" -scheme "+schemeName+" -configuration "+configurationName
    #清除编译的app
    xcodebuild_clean="xcodebuild clean"+baseCommand
    #查看项目全部的targets，schemes和configurations
    xcodebuild_list="xcodebuild -list"
    #编译
    xcodebuild_build="xcodebuild build"+baseCommand+" -derivedDataPath "+workspaceDirPath+"/build"
    #查看全部的SDK
    xcodebuild_showsdks="xcodebuild -showsdks"

    #archivePath命令
    #archivePathOption=" -archivePath "+archivePath+"/"+schemeName
    #exportPath命令
    #exportPathOption=" -exportPath "+exportPath+"/"
    #指定exportOptionsPlist文件路径
    #exportOptionsPlistOption=" -exportOptionsPlist "+workspaceDirPath+"/BuildAllApp.plist"
    #编译并生成.archive包
    #xcodebuild_archive="xcodebuild archive"+archivePathOption+baseCommand
    #.archive包导出ipa包
    #xcodebuild_exportArchive="xcodebuild -exportArchive"+archivePathOption+".xcarchive"+exportPathOption+exportOptionsPlistOption

    os.system(xcodebuild_clean)
    #os.system(xcodebuild_list)
    #os.system(xcodebuild_showsdks)
    os.system(xcodebuild_build)
    #os.system(xcodebuild_archive)
    #os.system(xcodebuild_exportArchive)

    #forend
'''