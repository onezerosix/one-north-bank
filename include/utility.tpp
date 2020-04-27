// =============================================================================
// File: utility.tpp
// =============================================================================
// Description:
//      This file is the implementation of the templated functions of the
//      utility namespace.
// =============================================================================
#include <iostream>
#include <limits>
#include <string>

template <class T> bool utility::get(T &input, std::string prompt_msg) {
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

template <class T> bool utility::get(T &input, T default_val, std::string prompt_msg) {
    using namespace std;

    bool successful = get(input, prompt_msg);
    if (!successful) {
        input = default_val;
    }

    return successful;
}