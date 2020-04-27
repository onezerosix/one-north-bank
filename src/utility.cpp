// =============================================================================
// File: utility.cpp
// =============================================================================
// Description:
//      This file is the implementation of the non-templated functions of the
//      utility namespace.
// =============================================================================
#include <ctime>
#include <iostream>
#include "Utilities.h"

using namespace std;

void utility::printDateAndTime() {
    // retrieve date & time
    time_t rawtime;
    time(&rawtime);

    cout << "Today's date and time is " << ctime(&rawtime) << endl;
}