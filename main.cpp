// =============================================================================
// File: main.cpp
// =============================================================================
// Description:
//      This program is for bank accounts. It uses a random access file.
// =============================================================================

#include <iostream>
#include "Bank.h"
using namespace std;

// global variable
static const string BANK_NAME = "One North Bank";
static const string RAF_NAME = "accounts.raf";
static const enum loginOptions = { // TODO: this..
    quit = 0,
    create_account = 1,
    login = 2
}

// function prototypes
int main();
void promptMenu();
bool loginRequested();

// ==== main ===================================================================
//
// =============================================================================
int main() {
    // TODO: this means you don't have to call delete (like new)?
    Bank bank = { RAF_NAME };

    int selection;
    Account account = NULL;

    while (selection = loginRequested()) {
        // TODO: probably a try catch
        if (selection == loginOptions::create_account) {
            account = bank.createAccount();
        }
        else {
            account = bank.login();
        }
        if (account == NULL) {
            // TODO: error msg
            continue;
        }

        promptMenu(bank, account)
    }

    cout << "Goodbye!\n";
    return 0;
}

// ==== promptMenu =============================================================
//
// =============================================================================
void promptMenu(Bank &bank, Account &account) { // TODO: this function
    bool logging_out = false;
    int selection;

    while (!logging_out) {
        cout << "\n1. View balance\n"
            << "2. Make a deposit\n"
            << "3. Make a withdrawal\n"
            << "4. Close account\n"
            << "5. Logout\n\n";

        made_selection = get(selection, -1, "Enter your menu choice: ");

        switch (selection) {
            case 1:
                bank.displayBalance(account);
                break;
            case 2:
                bank.deposit(account);
                break;
            case 3:
                bank.withdraw(account);
                break;
            case 4:
                bank.close(account);
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
        cout << "---------------------" << bankName << "-----------------\n"
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
                cout << "Invaild, try again";
                made_selection = false;
                break;
        }

        cout << endl;
    }

    return (loginOptions)selection; // TODO: not sure how to do this..
}