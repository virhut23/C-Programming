#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_USERS 100
#define MAX_TRANSACTIONS 100

struct Transaction {
    char type;  // 'D' for deposit, 'W' for withdrawal
    float amount;
};

struct SignUp {
    char user_name[255];
    char user_fatherName[255];
    char user_surname[255];
    int age;
    char security_question[255];
    char password[255];
    struct Transaction transactions[MAX_TRANSACTIONS];
    int transactionCount;
    float Current_Balance;
};

struct LogIn {
    char user_name[255];
    char user_surname[255];
    char password[255];
};

void saveUser(const struct SignUp *user);
int findUser(const char *username, const char *surname, struct SignUp *foundUser);
void signUp(struct SignUp *user);
int logIn(struct LogIn *logIn, struct SignUp *foundUser);
void performTransaction(struct SignUp *user);
void resetData();
void showHomeScreen();
void showLoggedInMenu();

int main() {
    char choice;
    struct SignUp users[MAX_USERS];
    int userCount = 0;
    struct LogIn loggedInUser;

    // Load existing user data from the file
    FILE *file = fopen("users.txt", "r");
    if (file != NULL) {
        while (userCount < MAX_USERS && fscanf(file, "%[^,],%[^,],%[^,],%d,%[^,],%[^\n]\n", // error 2
                                               users[userCount].user_name,
                                               users[userCount].user_fatherName,
                                               users[userCount].user_surname,
                                               &users[userCount].age,
                                               users[userCount].security_question,
                                               users[userCount].password) == 6) {
            fscanf(file, "%d%f", &users[userCount].transactionCount, &users[userCount].Current_Balance);
            for (int i = 0; i < users[userCount].transactionCount; i++) {
                fscanf(file, " %c%f", &users[userCount].transactions[i].type,
                       &users[userCount].transactions[i].amount);
            }

            userCount++;
        }
        fclose(file);
    }

    printf("Banking System\n");

    do {
        showHomeScreen();

        printf("Enter your choice: ");
        scanf(" %c", &choice);

        switch (toupper(choice)) {
            case 'S':
                signUp(&users[userCount]);
                userCount++;
                break;
            case 'L':
                if (logIn(&loggedInUser, users)) {
                    showLoggedInMenu();
                    performTransaction(&users[userCount - 1]);
                }
                break;
            case 'R':
                resetData();
                userCount = 0;
                break;
            case 'Q':
                printf("System Exit\n");
                break;
            default:
                printf("Invalid choice\n");
                break;
        }

    } while (toupper(choice) != 'Q');

    file = fopen("users.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < userCount; i++) {
            fprintf(file, "%s,%s,%s,%d,%s,%s\n",
                    users[i].user_name,
                    users[i].user_fatherName,
                    users[i].user_surname,
                    users[i].age,
                    users[i].security_question,
                    users[i].password);

            fprintf(file, "%d %f", users[i].transactionCount, users[i].Current_Balance);
            for (int j = 0; j < users[i].transactionCount; j++) {
                fprintf(file, " %c%f", users[i].transactions[j].type, users[i].transactions[j].amount);
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }

    return 0;
}

void saveUser(const struct SignUp *user) {
    FILE *file = fopen("users.txt", "a");
    if (file != NULL) {
        fprintf(file, "%s,%s,%s,%d,%s,%s\n",
                user->user_name,
                user->user_fatherName,
                user->user_surname,
                user->age,
                user->security_question,
                user->password);
        fclose(file);
    } else {
        printf("Error saving user.\n");
    }
}

int findUser(const char *username, const char *surname, struct SignUp *foundUser) {
    FILE *file = fopen("users.txt", "r");
    if (file != NULL) {
        while (fscanf(file, "%[^,],%[^,],%[^,],%d,%[^,],%[^\n]\n",
                      foundUser->user_name,
                      foundUser->user_fatherName,
                      foundUser->user_surname,
                      &foundUser->age,
                      foundUser->security_question,
                      foundUser->password) == 6) {
            if (strcmp(foundUser->user_name, username) == 0 && strcmp(foundUser->user_surname, surname) == 0) {
                fscanf(file, "%d%f", &foundUser->transactionCount, &foundUser->Current_Balance);
                for (int i = 0; i < foundUser->transactionCount; i++) { // error 1
                    fscanf(file, " %c%f", &foundUser->transactions[i].type, &foundUser->transactions[i].amount);
                }

                fclose(file);
                return 1; // User found
            }
        }
        fclose(file);
    }
    return 0; // User not found
}

void signUp(struct SignUp *user) {
    printf("Enter your name: ");
    scanf("%s", user->user_name);

    printf("Enter father name: ");
    scanf("%s", user->user_fatherName);

    printf("Enter your surname: ");
    scanf("%s", user->user_surname);

    struct SignUp foundUser;
    if (findUser(user->user_name, user->user_surname, &foundUser)) {
        printf("User with the same name and surname already exists.\n");
        return;
    }

    printf("Enter age: ");
    scanf("%d", &user->age);

    printf("Enter new password: ");
    scanf("%s", user->password);

    printf("Enter pets name (security question): ");
    scanf("%s", user->security_question);

    user->transactionCount = 0;
    user->Current_Balance = 0;

    saveUser(user);

    printf("Sign-up successful!\n");
}

int logIn(struct LogIn *logIn, struct SignUp *foundUser) {
    printf("Enter your name: ");
    scanf("%s", logIn->user_name);

    printf("Enter your surname: ");
    scanf("%s", logIn->user_surname);

    if (findUser(logIn->user_name, logIn->user_surname, foundUser)) {
        printf("Enter your password: ");
        scanf("%s", logIn->password);

        if (strcmp(logIn->password, foundUser->password) == 0) {
            printf("Login Successful!\n");
            return 1;
        } else {
            printf("Incorrect password. Login failed.\n");
        }
    } else {
        printf("User not found. Check your credentials.\n");
    }
    return 0;
}

void performTransaction(struct SignUp *user) {
    char choice;

    do {
        printf("Your current Balance is: %f $\n", user->Current_Balance);
        printf("Hit (D) for deposit and (W) for withdraw: ");
        scanf(" %c", &user->transactions[user->transactionCount].type);

        user->transactions[user->transactionCount].type = toupper(user->transactions[user->transactionCount].type);

        if (user->transactions[user->transactionCount].type == 'D') {
            printf("Enter your deposit amount: ");
            scanf("%f", &user->transactions[user->transactionCount].amount);
            user->Current_Balance += user->transactions[user->transactionCount].amount;
        } else if (user->transactions[user->transactionCount].type == 'W') {
            printf("Enter your withdrawal amount: ");
            scanf("%f", &user->transactions[user->transactionCount].amount);

            if (user->transactions[user->transactionCount].amount > user->Current_Balance) {
                printf("Not enough money!\n");
            } else {
                user->Current_Balance -= user->transactions[user->transactionCount].amount;
            }
        } else {
            printf("Invalid choice\n");
        }

        user->transactionCount++;

        printf("Do you want to perform another transaction? (y/n): ");
        scanf(" %c", &choice);

    } while (toupper(choice) == 'Y');
}

void resetData() {
    FILE *file = fopen("users.txt", "w");
    if (file != NULL) {
        fclose(file);
        printf("\nData has been reset.\n");
    } else {
        printf("Error resetting data.\n");
    }
}

void showHomeScreen() {
    printf("\n=== Banking System ===\n");
    printf("S - Sign Up\n");
    printf("L - Log In\n");
    printf("R - Reset Data\n");
    printf("Q - Quit\n");
}

void showLoggedInMenu() {
    printf("\n=== Logged In ===\n");
}

