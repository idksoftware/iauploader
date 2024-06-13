#pragma region Includes
#include <string>
#include <cstdio>
#include <windows.h>
#include "svcbase/ServiceInstaller.h"
#include "svcbase/ServiceBase.h"
#include "SOCClientService.h"
#pragma endregion


// 
// Settings of the service
// 

// Internal name of the service
#define SERVICE_NAME             "SOCSwitchClient"

// Displayed name of the service
#define SERVICE_DISPLAY_NAME     "SOC Switch Service"

// Service start options.
#define SERVICE_START_TYPE       SERVICE_DEMAND_START

// List of service dependencies - "dep1\0dep2\0\0"
#define SERVICE_DEPENDENCIES     ""

// The name of the account under which the service should run
#define SERVICE_ACCOUNT          "NT AUTHORITY\\LocalService"

// The password to the service account name
#define SERVICE_PASSWORD         nullptr

#define SERVICE_DESCRITION      "Stratospheric Operations Centre Switch Client Service"
//
//  FUNCTION: wmain(int, wchar_t *[])
//
//  PURPOSE: entrypoint for the application.
// 
//  PARAMETERS:
//    argc - number of command line arguments
//    argv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    wmain() either performs the command line task, or run the service.
//
int main(int argc, char* argv[])
{
    if ((argc > 1) && ((*argv[1] == '-' || (*argv[1] == '/'))))
    {
        std::string arg = argv[1] + 1;
        if (arg.compare("install") == 0)
        {
            // Install the service when the command is 
            // "-install" or "/install".
            InstallService(
                SERVICE_NAME,               // Name of service
                SERVICE_DISPLAY_NAME,       // Name to display
                SERVICE_START_TYPE,         // Service start type
                SERVICE_DEPENDENCIES,       // Dependencies
                SERVICE_ACCOUNT,            // Service running account
                SERVICE_PASSWORD,            // Password of the account
                SERVICE_DESCRITION
            );
        }
        else if (arg.compare("remove") == 0)
        {
            // Uninstall the service when the command is 
            // "-remove" or "/remove".
            UninstallService(SERVICE_NAME);
        }
    }
    else
    {
        printf("Parameters:\n");
        printf(" -install  to install the service.\n");
        printf(" -remove   to remove the service.\n");

        SOCClientService service(SERVICE_NAME);
        if (!CServiceBase::Run(service))
        {
            printf("Service failed to run w/err 0x%08lx\n", GetLastError());
        }
    }

    return 0;
}

