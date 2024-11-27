/**
 *
 *  @file      ComLib.cpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 */
#include "ComLib.hpp"

#include <objbase.h>

ComLib::ComLib() { CoInitializeEx(nullptr, COINIT_MULTITHREADED); }

ComLib::~ComLib() { CoUninitialize(); }