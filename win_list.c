#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <WinSock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib") 

int main() {
    
    WSADATA d;
    
    if(WSAStartup(MAKEWORD(2, 2), &d)) {
        printf("Failed to initialized.\n");
        return -1;
    }

    DWORD asize = 20000; // allocate 20 000 bytes to adapters
    PIP_ADAPTER_ADDRESSES adapters;

    do {
        adapters = (PIP_ADAPTER_ADDRESSES)malloc(asize); // function malloc() returns 0 if it fails.

        if(!adapters) {
            printf("couldn't allocate %ld bytes for adapters.\n", asize);
            WSACleanup();
            return -1;
        }

        int r = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, 0, adapters, &asize); // AF_UNSPEC tells that I want both IPv4 and IPv6 - GAA_FLAG_INCLUDE_PREFIX request a list of addresses

        if(r == ERROR_SUCCESS) {
            break;
        }

        else {
            printf("Error from GetAdaptersAddresses: %d\n", r);
            free(adapters);
            WSACleanup();
            return -1;
        }
    } while(!adapters);

    PIP_ADAPTER_ADDRESSES adapter = adapters; //  walk through the linked list of adapters

    while(adapter) {
        printf("\nAdapter name: %S\n", adapter->FriendlyName); // Friendly name sets a name i guess?? 

        PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress;
        while(address) {
            printf("\t%s", address->Address.lpSockaddr->sa_family == AF_INET ? "IPv4" : "IPv6");

            char ap[100];

            getnameinfo(address->Address.lpSockaddr, address->Address.iSockaddrLength, ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
            printf("\t%s\n", ap);

            address = address->Next; // Gets to the next address
        }
        adapter = adapter->Next; // Gets to the next adapter
    }


    free(adapters);
    WSACleanup();
    return 0;

}