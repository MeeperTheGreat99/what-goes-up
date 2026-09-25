#pragma once
#include <exception>

typedef const std::exception& exc;

void reportException(exc e);