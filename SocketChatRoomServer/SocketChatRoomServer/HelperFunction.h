#pragma once
#include <iphlpapi.h>
#include <stdlib.h>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

std::string GetIPAddress() {

    std::string result;

    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    UINT i;
    struct tm newtime;
    char buffer[32];
    errno_t error;

    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        return "-1";
    }
    // Make an initial call to GetAdaptersInfo to get
    // the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            return "-1";
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        /*printf("\tIP Address: \t%s\n",
            pAdapter->IpAddressList.IpAddress.String);*/
        result = std::string(pAdapter->IpAddressList.IpAddress.String);
    }
    else {
        printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);

    }
    if (pAdapterInfo)
        FREE(pAdapterInfo);
    if (result == "0.0.0.0") {
        result = "127.0.0.1";
    }
    return result;
}