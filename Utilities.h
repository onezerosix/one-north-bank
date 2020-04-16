// =============================================================================
// File: Utilities.h
// =============================================================================
// Description:
//      This header file hosts utility functions.
// =============================================================================

#ifndef UTILITIES_H
#define UTILITIES_H

#include <ctime>
#include <iostream>
using namespace std;

// function prototypes
template <class T> bool get(T &input, T default_val, string prompt);
void printDateAndTime();

// === get =====================================================================
// This function gets user input.
//
// Input:
//      input [IN/OUT]          -- variable where input will be saved
//      default_val [IN]        -- default value to set if an error occurred
//      prompt_msg [OPT IN]     -- optional: message to show right before asking
//                                      the user for input
//
// Output:
//      true if no errors occured, otherwise false
// =============================================================================
template <class T> bool get(T &input, T default_val, string prompt_msg = NULL) {
    if (prompt != NULL) {
        cout << prompt;
    }

    bool successful = true;

    cin >> input;

    if (cin.fail()) {
        successful = false;
        input = default_val;
    }

    // clear buffer
    cin.clear();
    cin.ignore(numeric_limits<streamsize>, '\n');

    return successful;
}

// ==== printDateAndTime =======================================================
// This function prints the date and time.
//
// Input: None
//
// Output: None
// =============================================================================
void printDateAndTime() {
    // retrieve date & time
    time_t time;
    time(&time);

    cout << "Today's date and time is " << ctime(&time) << endl;
}

#endif // UTILITIES_H