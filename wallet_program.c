#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// SQLite database connection
sqlite3 *db;

// Function to initialize SQLite database
void init_db() {
    int rc = sqlite3_open("wallets.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
}

// Function to perform database cleanup
void cleanup_db() {
    sqlite3_close(db);
}

// Function to send money from one wallet to another
void send_money(int sender_id, int receiver_id, float amount) {
    // Retrieve sender's balance
    char *sql = "SELECT balance FROM wallets WHERE user_id = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, sender_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        fprintf(stderr, "User ID %d not found\n", sender_id);
        sqlite3_finalize(stmt);
        return;
    }
    double sender_balance = sqlite3_column_double(stmt, 0);
    sqlite3_finalize(stmt);

    // Check if sender has sufficient balance
    if (sender_balance < amount) {
        fprintf(stderr, "Insufficient balance for user ID %d\n", sender_id);
        return;
    }

    // Update sender's balance
    sql = "UPDATE wallets SET balance = balance - ? WHERE user_id = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_double(stmt, 1, amount);
    sqlite3_bind_int(stmt, 2, sender_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    // Update receiver's balance
    sql = "UPDATE wallets SET balance = balance + ? WHERE user_id = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_double(stmt, 1, amount);
    sqlite3_bind_int(stmt, 2, receiver_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    // Log transaction
    sql = "INSERT INTO transactions (sender_id, receiver_id, amount) VALUES (?, ?, ?)";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, sender_id);
    sqlite3_bind_int(stmt, 2, receiver_id);
    sqlite3_bind_double(stmt, 3, amount);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    printf("Transaction successful: %.2f sent from user %d to user %d.\n", amount, sender_id, receiver_id);
}

// Function to display wallet balances of all users
void display_balances() {
    // Execute SQL query to retrieve wallet balances of all users
    char *sql = "SELECT user_id, balance FROM wallets ORDER BY user_id";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Fetch and print wallet balances
    printf("User ID\t\tBalance\n");
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int user_id = sqlite3_column_int(stmt, 0);
        double balance = sqlite3_column_double(stmt, 1);
        printf("%d\t\t%.2f\n", user_id, balance);
    }

    // Finalize statement
    sqlite3_finalize(stmt);
}

int main() {
    // Initialize database
    init_db();

    // Variables for user input
    int sender_id, receiver_id;
    float amount;

    // Prompt user for transaction details
    printf("Enter sender ID: ");
    scanf("%d", &sender_id);
    printf("Enter receiver ID: ");
    scanf("%d", &receiver_id);
    printf("Enter amount to send: ");
    scanf("%f", &amount);

    // Perform transaction
    send_money(sender_id, receiver_id, amount);

    // Display wallet balances after transaction
    printf("User ID\t\tBalance\n");
    display_balances();

    // Cleanup database
    cleanup_db();

    return 0;
}
