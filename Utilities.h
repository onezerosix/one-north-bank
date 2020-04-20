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
#include <limits>
#include <string>

// function prototypes
template <class T> bool get(T &input, string prompt);
template <class T> bool get(T &input, T default_val, string prompt);
void printDateAndTime();

// === get =====================================================================
// This function gets user input.
//
// Input:
//      input [IN/OUT]          -- variable where input will be saved
//      prompt_msg [OPT IN]     -- optional: message to show right before asking
//                                      the user for input. ignored if empty
//
// Output:
//      true if no errors occured, otherwise false
// =============================================================================
template <class T> bool get(T &input, string prompt_msg) {
    using namespace std;

    if (!prompt_msg.empty()) {
        cout << prompt_msg;
    }

    bool successful = true;

    cin >> input;

    if (cin.fail()) {
        successful = false;
    }

    // clear buffer
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    return successful;
}

// === get =====================================================================
// This function gets user input and sets a default value if an error occurred.
//
// Input:
//      input [IN/OUT]          -- variable where input will be saved
//      default_val [IN]        -- default value to set if an error occurred
//      prompt_msg [OPT IN]     -- optional: message to show right before asking
//                                      the user for input. ignored if empty
//
// Output:
//      true if no errors occured, otherwise false
// =============================================================================
template <class T> bool get(T &input, T default_val, string prompt_msg) {
    using namespace std;

    bool successful = get(input, prompt_msg);
    if (!successful) {
        input = default_val;
    }

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
    using namespace std;

    // retrieve date & time
    time_t rawtime;
    time(&rawtime);

    cout << "Today's date and time is " << ctime(&rawtime) << endl;
}

#endif // UTILITIES_H