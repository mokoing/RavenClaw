//=============================================================================
//
//   Copyright (c) 2000-2004, Carnegie Mellon University.  
//   All rights reserved.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer. 
//
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in
//      the documentation and/or other materials provided with the
//      distribution.
//
//   This work was supported in part by funding from the Defense Advanced 
//   Research Projects Agency and the National Science Foundation of the 
//   United States of America, and the CMU Sphinx Speech Consortium.
//
//   THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
//   ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
//   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//   PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
//   NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//=============================================================================

//-----------------------------------------------------------------------------
// 
// LOG.CPP    - declarations of functions for logging support
// 
// ----------------------------------------------------------------------------
// 
// BEFORE MAKING CHANGES TO THIS CODE, please read the appropriate 
// documentation, available in the Documentation folder. 
//
// ANY SIGNIFICANT CHANGES made should be reflected back in the documentation
// file(s)
//
// ANY CHANGES made (even small bug fixes, should be reflected in the history
// below, in reverse chronological order
// 
// HISTORY --------------------------------------------------------------------
//
//   [2004-03-15] (dbohus):  fixed bug in logging long strings to the screen
//   [2003-05-13] (dbohus):  changed InitializeLogging function to work with 
//                            the new configuration parameters
//   [2003-04-09] (dbohus, 
//                 antoine): added dialog task logging stream
//   [2002-12-05] (dbohus):  added support for colored logging
//   [2002-10-20] (dbohus):  changed initialize function so that streams are 
//                            added according to specified parameters
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-03-21] (dbohus):  added support for multiple logging streams
//   [2002-01-02] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __LOG_H__
#define __LOG_H__

#include "Core.h"
#include "../Utils/Utils.h"

//-----------------------------------------------------------------------------
// Logging streams: definition and functions
//-----------------------------------------------------------------------------

// D: defines for the predefined (always on) logging streams
// 定义预定义（始终打开）日志记录流
#define ERROR_STREAM "ERR" 
#define WARNING_STREAM "WAR"
#define FATALERROR_STREAM "FATAL_ERROR"

// D: the core thread logging stream 
#define CORETHREAD_STREAM "CTH"				//核心线程日志流
// D: the registry logging stream 
#define REGISTRY_STREAM "REG"				//注册表日志流
// D: the core logging streams  
#define DMCORE_STREAM "COR"					//核心日志流
#define EXPECTATIONAGENDA_STREAM "AGN"		//期望Agenda日志流
// D: the dialog task tree manager logging stream 
#define DTTMANAGER_STREAM "DTT"				//对话任务树日志流
// D: the input manager logging stream 
#define INPUTMANAGER_STREAM "INP"			//输入manager日志流
// D: the Output Manager Logging Stream 
#define OUTPUTMANAGER_STREAM "OTP"			//输出manager日志流
// D: the Output History Logging Stream 
#define OUTPUTHISTORY_STREAM "OTH"			//输入历史日志流
// D: the state manager logging stream 
#define STATEMANAGER_STREAM "STA"			//状态manager日志流
// D: the logging streams for the TrafficManagerAgent 
#define TRAFFICMANAGER_STREAM "TMS"			//‘交通’manager Agent日志流
#define TRAFFICMANAGERDUMP_STREAM "TMD"
// D: the logging streams for the GroundingManagerAgent 
#define GROUNDINGMANAGER_STREAM "GMS"		//‘落地’manager Agent日志流
#define GROUNDINGMODELX_STREAM "GMX"
// D: define the belief updating stream 
#define BELIEFUPDATING_STREAM "BUS"			//置信度更新日志流
// D: the dialog task logging stream 
#define DIALOGTASK_STREAM "DTS"				//对话任务日志流
// D: the concept logging stream 
#define CONCEPT_STREAM "CON"				//上下文日志流

// D: string holding the names of all the other logging streams
extern string sAllLoggingStreams;

// D: structure capturing the properties of a logging stream
// # 日志流的属性的结构
typedef struct
{
	bool bDisplayed;		// if that stream should be displayed or not	//是否可显示
	int iColor;             // the color that stream is displayed			//显示的字体颜色
	bool bEnabled;			// if the logging stream is enabled or not		//是否有效[可用]
} TLoggingStream;

// D: definition for the hash storing the logging streams
// #定义存储日志流的Hash表
typedef map <string, TLoggingStream, less<string>, allocator<TLoggingStream> >
TLoggingStreamsHash;

// D: the actual hash storing the logging streams
// 实际的hash，存储日志流
static TLoggingStreamsHash lshLogStreams;

// D: Add a logging stream
void AddLoggingStream(string sStreamName,
	string sColorMap = "",
	bool bDisplayed = true,
	bool bEnabled = true);

// D: Delete a logging stream
void DeleteLoggingStream(string sStreamName);

// D: Enable a logging stream
void EnableLoggingStream(string sStreamName);

// D: Disable a logging stream
void DisableLoggingStream(string sStreamName);

// D: Enable all logging streams
void EnableAllLoggingStreams();

// D: Disable all logging streams
void DisableAllLoggingStreams();


//-----------------------------------------------------------------------------
// Logging Functions
//-----------------------------------------------------------------------------

// D: the log folder. By default, it is the current folder
static string sLogFolder = ".\\";			// needs to end with a backslash

// D: the log filename. By default, it is dialog.log
// 默认log文件名
#define DEFAULT_LOG_FILENAME "dialog.log"
static string sLogFilename = DEFAULT_LOG_FILENAME;

// D: the log FILE handle
static FILE* fileLog = NULL;

// D: variants of the log function (works like a printf)
int Log(string sLoggingStream, const char* lpszFormat, ...);
int Log(string sLoggingStream, string sMessage);

// D: logging initialization function
void InitializeLogging(string sALogFolder, string sALogPrefix, string sALogFilename,
	string sLoggedStreamsList, string sDisplayedStreamsList,
	string sAExitOnFatalError);

// D: logging closing function
void TerminateLogging();

//-----------------------------------------------------------------------------
// Error handling functions 
//   these functions are basically stubs for the Log function
//-----------------------------------------------------------------------------

// D: Warning
void __Warning(const char* lpszWarning, char* lpszFile, int iLine);
// D: macro to be used to record the Warning
#define Warning(sWarning) \
	__Warning(string(sWarning).c_str(), __FILE__, __LINE__)

// D: NonFatal error. 
void __Error(const char* lpszError, char* lpszFile, int iLine);
// D: macro to be used to record the error
#define Error(sError) \
	__Error(string(sError).c_str(), __FILE__, __LINE__)

// D: Fatal error. 
void __FatalError(const char* lpszError, char* lpszFile, int iLine);
// D: macro to be used to record a FatalError
#define FatalError(sError) \
	__FatalError(string(sError).c_str(), __FILE__, __LINE__)

#endif // __LOG_H__