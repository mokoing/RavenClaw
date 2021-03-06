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
// MAREQUEST.CPP - implementation of the CMARequest class. This class 
//				   implements the microagent for Request
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
//   [2005-10-24] (antoine): removed RequiresFloor method (the method inherited
//							 from CDialogAgent is now valid here)
//   [2005-10-19] (antoine): added RequiresFloor method
//   [2004-12-23] (antoine): modified constructor, agent factory to handle
//							  configurations
//   [2004-04-16] (dbohus):  added grounding models on dialog agents
//   [2003-04-25] (dbohus,
//                 antoine): the request agent doesn't check for completion on 
//                            execute any more. the core takes care of that
//   [2003-04-08] (dbohus):  change completion evaluation on execution
//   [2002-11-20] (dbohus):  changed behavior so that it's an "open request" if
//                            no requested concept is specified
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-03-20] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#include "MARequest.h"
#include "../../../../DMCore/Core.h"

//-----------------------------------------------------------------------------
//
// D: the CMARequest class -- the microagent for Request
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructors and Destructors
//-----------------------------------------------------------------------------

// D: default constructor
CMARequest::CMARequest(string sAName, string sAConfiguration, string sAType) :
CDialogAgent(sAName, sAConfiguration, sAType)
{
}

// D: virtual destructor - does nothing
CMARequest::~CMARequest()
{
}

//-----------------------------------------------------------------------------
// D: Static function for dynamic agent creation
//-----------------------------------------------------------------------------
CAgent* CMARequest::AgentFactory(string sAName, string sAConfiguration)
{
	return new CMARequest(sAName, sAConfiguration);
}

//-----------------------------------------------------------------------------
//
// Specialized (overwritten) CDialogAgent methods
//
//-----------------------------------------------------------------------------

// D: Execute function: issues the request prompt, then waits for user input
// D：执行函数：发出请求prompt提示，【然后等待用户输入input】
TDialogExecuteReturnCode CMARequest::Execute()
{

	//		call on the output manager to send out the output
	// <1>	调用输出管理器发出输出 
	//		floor = fsUser 等待用户输入
	pOutputManager->Output(this, Prompt(), fsUser);

	//		set the timeout period
	// <2>	设置超时时间
	pDMCore->SetTimeoutPeriod(GetTimeoutPeriod());

	//		set the nonunderstanding threshold
	// <3>	设置nonunderstanding的阈值
	pDMCore->SetNonunderstandingThreshold(GetNonunderstandingThreshold());

	//		increment the execute counter
	// <4>	递增执行计数器
	IncrementExecuteCounter();

	// and return with 
	return dercTakeFloor;//保持Floor
}

// A: Resets clears the list of outputs for this concept
void CMARequest::Reset()
{
	CDialogAgent::Reset();
	voOutputs.clear();
}

// D: Declare the expectations: add to the incoming list the expectations 
//    of this particular agent, as specified by the GrammarMapping slot
// D：Declare the expectations：
//	  添加到传入列表这个特定代理的期望，由GrammarMapping slot指定
int CMARequest::DeclareExpectations(TConceptExpectationList& celExpectationList)
{

	int iExpectationsAdded = 0;
	TConceptExpectationList celLocalExpectationList;
	bool bExpectCondition = ExpectCondition();//宏定义 #define EXPECT_WHEN(Condition)

	//		first get the expectations from the local "expected" concept
	// <1>	首先从当地的“预期”概念得到期望
	string sRequestedConceptName = RequestedConceptName();	//宏定义 “REQUEST_CONCEPT（xxx）”
	string sGrammarMapping = GrammarMapping();				//宏定义 #define GRAMMAR_MAPPING("![Yes]>true, ![No]>false")

	// <2>	非空，解析
	if (!sRequestedConceptName.empty() && !sGrammarMapping.empty())
	{
		parseGrammarMapping(sRequestedConceptName, sGrammarMapping, celLocalExpectationList);
	}
	//		now go through it and add stuff to the 
	// <3>	遍历收集结果
	for (unsigned int i = 0; i < celLocalExpectationList.size(); i++)
	{
		//		if the expect condition is not satisfied, disable this 
		//		expectation and set the appropriate reason
		//<4>	如果期望条件不满足，请禁用此期望值并设置适当的原因
		if (!bExpectCondition)
		{
			celLocalExpectationList[i].bDisabled = true;
			celLocalExpectationList[i].sReasonDisabled = "expect condition false";
		}
		// <5>	添加到列表
		celExpectationList.push_back(celLocalExpectationList[i]);
		iExpectationsAdded++;
	}

	//		now add whatever needs to come from the CDialogAgent side
	//		现在添加来自CDialogAgent端的任何需求
	// <6>	调用父类方法 - 解析 TriggeredByCommands()
	iExpectationsAdded += CDialogAgent::DeclareExpectations(celExpectationList);

	//		and finally return the total number of added expectations
	// <7>	最后返回添加的期望的总数
	return iExpectationsAdded;
}

// D: Preconditions: by default, preconditions for a request agent are that 
//    the requested concept is not available
bool CMARequest::PreconditionsSatisfied()
{
	if (RequestedConceptName() != "")
		return !RequestedConcept()->IsAvailableAndGrounded();
	else
		return true;
}

// D: SuccessCriteriaSatisfied: Request agents are completed when the concept 
//	  has become grounded
// 当概念变得接地时，请求代理完成
bool CMARequest::SuccessCriteriaSatisfied()
{
	if (RequestedConceptName() != "")//如果有concept，接地返回True
		return RequestedConcept()->IsUpdatedAndGrounded();
	else
	{
		// if it's an open request, then it completes when it's been tried and 
		// some concept got bound in the previous input pass
		//如果它是一个打开的请求，那么当它被尝试和一些概念在前面的输入传递绑定完成
		return ((iTurnsInFocusCounter > 0) && !pDMCore->LastTurnNonUnderstanding());//尝试执行过，并且上一轮用户输入是理解的的
	}
}

// A: FailureCriteriaSatisfied: Request agents fail when the maximum number
//    of attempts has been made
// A：FailureCriteriaSatisfied：请求代理在尝试最大尝试次数时失败
bool CMARequest::FailureCriteriaSatisfied()//#define FAILS_WHEN(Condition)
{
	bool bFailed = (iTurnsInFocusCounter >= GetMaxExecuteCounter()) &&
		!SuccessCriteriaSatisfied();

	if (bFailed)
		Log(DIALOGTASK_STREAM, "Agent reached max attempts (%d >= %d), failing",
		iTurnsInFocusCounter, GetMaxExecuteCounter());

	return bFailed;
}

// D: Returns the request prompt as a string
// D：以字符串形式返回请求提示
string CMARequest::Prompt()
{
#ifdef GALAXY
	// by default, request the name of the requested concept
	if(RequestedConceptName() != "") {
		// get the first requested concept
		string sConceptName, sFoo;
		SplitOnFirst(RequestedConceptName(), ";", sConceptName, sFoo);
		return FormatString("{request %s agent=%s}", sConceptName.c_str(), 
			sName.c_str());
	} else
		return FormatString("{request generic agent=%s}", sName.c_str());
#endif
#ifdef OAA
	// by default return the name of the requested concept
	if(RequestedConceptName() != "") {
		// get the first requested concept
		string sConceptName, sFoo;
		SplitOnFirst(RequestedConceptName(), ";", sConceptName, sFoo);
		return FormatString("[%s]", sConceptName.c_str());
	} else 
		return "[generic]";
#endif
}


//-----------------------------------------------------------------------------
//
// Request Microagent specific methods
//
//-----------------------------------------------------------------------------

// D: Returns a string describing the concept mapping (nothing for this class)
//    Is to be overwritten by derived classes
// D：返回一个描述概念映射的字符串（这个类没有任何东西）被派生类覆盖
//#define GRAMMAR_MAPPING(GrammarMappingAsString)
string CMARequest::GrammarMapping()
{
	return "";
}

// D: Returns the name of the requested concept (nothing for this class); is to 
//    be overwritten by derived classes
// D：返回请求的概念的名称（对于这个类没有任何东西）; 将被派生类覆盖
//		宏定义 “REQUEST_CONCEPT（xxx）”
string CMARequest::RequestedConceptName()
{
	return "";
}

// D: Returns a pointer to the requested concept (uses the concept name)
// D：返回指向所请求概念的指针（使用概念名称）
CConcept* CMARequest::RequestedConcept()
{
	if (RequestedConceptName() != "")
	{
		// get the first requested concept
		string sConceptName, sFoo;
		SplitOnFirst(RequestedConceptName(), ";", sConceptName, sFoo);
		return &C(sConceptName);
	}
	else return &NULLConcept;
}

// D: Retuns the timeout duration : by default it returns whatever the 
//    default value is in the DMCore
int CMARequest::GetTimeoutPeriod()
{
	return pDMCore->GetDefaultTimeoutPeriod();
}

// D: Retuns the nonunderstanding threshold: by default it returns whatever the 
//    default value is in the DMCore
float CMARequest::GetNonunderstandingThreshold()
{
	return pDMCore->GetDefaultNonunderstandingThreshold();
}
