// =============================================================================
// File: main.cpp
// =============================================================================
// Description:
//      This program is for bank accounts. It uses a random access file.
// =============================================================================

#include <iostream>
#include <memory>
#include "Bank.h"
#include "utility.h"
using namespace std;
using namespace utility;

// global variable
static const string BANK_NAME = "One North Bank";
static const string RAF_NAME = "accounts.raf";
// static const enum loginOptions = { // TODO: this..
//     quit = 0,
//     create_account = 1,
//     login = 2
// };

// function prototypes
int main();
void promptMenu(Bank &bank);
int loginRequested();

// ==== main ===================================================================
//
// =============================================================================
int main() {
    Bank bank(RAF_NAME);

    int selection;
    bool logged_in = false;

    while (selection = loginRequested()) {
        // TODO: probably a try catch
        if (selection == 1) {
            logged_in = bank.createAccount();
        }
        else {
            logged_in = bank.login();
        }
        if (!logged_in) {
            // TODO: error msg
            continue;
        }

        promptMenu(bank);
    }

    cout << "Goodbye!\n";
    return 0;
}

// ==== promptMenu =============================================================
//
// =============================================================================
void promptMenu(Bank &bank) { // TODO: this function
    bool logging_out = false;
    int selection;

    while (!logging_out) {
        cout << "\n1. View balance\n"
            << "2. Make a deposit\n"
            << "3. Make a withdrawal\n"
            << "4. Close account\n"
            << "5. Logout\n\n";

        get(selection, -1, "Enter your menu choice: ");

        switch (selection) {
            case 1:
                bank.displayBalance();
                break;
            case 2:
                bank.adjustBalance(true);
                break;
            case 3:
                bank.adjustBalance(false);
                break;
            case 4:
                bank.closeAccount();
                logging_out = true;
                break;
            case 5:
                // TODO: msg?
                logging_out = true;
                break;
            default:
                cout << "Invaild menu choice, try again\n";
                break;
        }

        cout << endl;
    }
}

// ==== loginRequested =========================================================
//
// =============================================================================
int loginRequested() {
    printDateAndTime();

    bool made_selection = false;
    int selection;

    while (!made_selection) {
        cout << "---------------------" << BANK_NAME << "-----------------\n"
            << "1. Create account\n"
            << "2. Login\n"
            << "3. Quit\n";

        made_selection = get(selection, -1, "Enter your choice: ");

        switch (selection) {
            case 1:
            case 2:
                break;
            case 3:
                selection = 0;
                break;
            default:
                cout << "Invaild, try again\n";
                made_selection = false;
                break;
        }

        cout << endl;
    }

    return selection;
}