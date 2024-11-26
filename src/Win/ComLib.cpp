#include <objbase.h>
#include "ComLib.hpp"

ComLib::ComLib() 
{
  CoInitializeEx(nullptr, COINIT_MULTITHREADED);
}

ComLib::~ComLib()
{
  CoUninitialize();
}