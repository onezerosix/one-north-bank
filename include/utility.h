// =============================================================================
// File: utility.h
// =============================================================================
// Description:
//      This header file hosts the utility namespace.
// =============================================================================

#ifndef UTILITY_H
#define UTILITY_H

#include <string>

namespace utility {
    using namespace std;

    // === get =================================================================
    // This function gets user input.
    //
    // Parameters:
    //      input [OUT]                 -- variable where input will be saved
    //      prompt_msg [OPT IN]         -- optional: message to show before
    //                                      asking the user for input. ignored
    //                                      if empty
    //
    // Return val:
    //      true if no errors occured, otherwise false
    // =========================================================================
    template <class T> bool get(T &input, string prompt);

    // === get =================================================================
    // This function gets user input and sets a default value if an error
    // occurred.
    //
    // Parameters:
    //      input [OUT]                 -- variable where input will be saved
    //      default_val [IN]            -- default value to set if an error
    //                                      occurred
    //      prompt_msg [OPT IN]         -- optional: message to show before
    //                                      asking the user for input. ignored
    //                                      if empty
    //
    // Return val:
    //      true if no errors occured, otherwise false
    // =========================================================================
    template <class T> bool get(T &input, T default_val, string prompt);

    // ==== printDateAndTime ===================================================
    // This function prints the date and time.
    //
    // Parameters: None
    //
    // Return val: None
    // =========================================================================
    void printDateAndTime();
}

#include "utility.tpp"

#endif // UTILITY_H