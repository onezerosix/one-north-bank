// TODO: want to: encrypt or salt lines of raf with user password

// // TODO: incomplete file & documentaiton..

// bool getLogin() {
    

//     string name;
//     get(name, NULL, "Enter your name: ");
//     santize(name);

//     if (name == NULL) {
//         printErrorMsg();
//         return false;
//     }

//     bool username_exists = !first_run && getLogin();

//     if (first_run || !username_exists) {
//         return createNewAccount();
//     }
//     else {
//         return validatePassword();
//     }
// }