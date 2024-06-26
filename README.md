# Wallet Transaction System

This program facilitates transactions between users by updating their wallet balances stored in an SQLite database.

## Description

The program consists of C code that interacts with an SQLite database to perform wallet transactions. Users can send money from one wallet to another by specifying the sender ID, receiver ID, and the amount to send. The program ensures that the sender has sufficient balance before executing the transaction and logs the transaction details in the database.

## Installation

1. Ensure you have SQLite installed on your system.
2. Compile the C code using a C compiler.
3. Execute the compiled binary to run the program.

## Usage

1. Run the compiled binary.
2. Enter the sender ID, receiver ID, and the amount to send when prompted.
3. The program will perform the transaction and display the updated wallet balances.

## Files

- `wallet_transaction.c`: Contains the main C code for the wallet transaction system.
- `wallets.db`: SQLite database file storing wallet balances and transaction history.

## Dependencies

- `sqlite3.h`: SQLite header file for C programming.

## Contributors

- Humphrey Kyalo

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
