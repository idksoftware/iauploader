/****************************** Module Header ******************************\
* Module Name:  SampleService.h
* Project:      CppWindowsService
* Copyright (c) Microsoft Corporation.
*
* Provides a sample service class that derives from the service base class -
* CServiceBase. The sample service logs the service start and stop
* information to the Application event log, and shows how to run the main
* function of the service in a thread pool worker thread.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/en-us/openness/resources/licenses.aspx#MPL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once

#include "svcbase/ServiceBase.h"


class SOCClientService : public CServiceBase
{
public:

    SOCClientService(const char* pszServiceName,
        bool fCanStop = true,
        bool fCanShutdown = true,
        bool fCanPauseContinue = true);
    virtual ~SOCClientService(void) override;

protected:

    virtual void OnStart(DWORD dwArgc, const char*& pszArgv) override;
    virtual void OnStop() override;

    void ServiceWorkerThread(void);

private:

    bool m_fStopping;
    HANDLE m_hStoppedEvent;
};
