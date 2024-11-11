#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <random>
#include <cmath>
#include <unordered_map>

using namespace std;

const int P = 11; // Prime number for ZKP
const int G = 2;  // Generator for ZKP
const int DIFFICULTY = 2; // Difficulty for mining (number of leading zeros)

// Struct to represent a transaction
struct Transaction {
    string sender;
    string receiver;
    double fractional_ownership; // Percentage of ownership transferred
    string assetID;              // Unique identifier for the asset
    int sensitive_data;           // Secret data to verify in ZKP
};

// Class to represent a block
class Block {
public:
    int index;
    time_t timestamp;
    vector<Transaction> transactions;
    string previousHash;
    string hash;
    int nonce;

    Block(int idx, vector<Transaction> txs, string prevHash)
        : index(idx), transactions(txs), previousHash(prevHash), nonce(0) {
        timestamp = time(0);
        hash = calculateHash();
    }

    // Function to calculate the hash of the block (simplified)
    string calculateHash() const {
        return to_string(index) + to_string(timestamp) + to_string(nonce) + previousHash;
    }

    // Proof of Work method for mining
    void mineBlock() {
        while (hash.substr(0, DIFFICULTY) != string(DIFFICULTY, '0')) {
            nonce++;
            hash = calculateHash();
        }
        cout << "Block mined with hash: " << hash << endl;
    }
};

// Blockchain class
class Blockchain {
public:
    Blockchain() {
        // Create genesis block
        chain.push_back(createBlock(vector<Transaction>(), "0"));
    }

    void addTransaction(Transaction tx) {
        if (verifyTransaction(tx)) {
            pendingTransactions.push_back(tx);
        } else {
            cout << "Transaction failed verification!" << endl;
        }
    }

    void minePendingTransactions() {
        Block newBlock(chain.size(), pendingTransactions, chain.back().hash);
        newBlock.mineBlock();
        chain.push_back(newBlock);
        pendingTransactions.clear();
    }

    void viewAssetTransactions(string assetID) const {
        cout << "Transactions for Asset ID " << assetID << ":\n";
        for (const Block &block : chain) {
            for (const Transaction &tx : block.transactions) {
                if (tx.assetID == assetID) {
                    cout << "From: " << tx.sender << ", To: " << tx.receiver
                         << ", Ownership: " << tx.fractional_ownership << "%" << endl;
                }
            }
        }
    }

private:
    vector<Block> chain;
    vector<Transaction> pendingTransactions;

    Block createBlock(vector<Transaction> txs, string prevHash) {
        return Block(chain.size(), txs, prevHash);
    }

    bool verifyTransaction(Transaction tx) {
        // Implementing Zero-Knowledge Proof for sensitive data verification
        int x = tx.sensitive_data;
        int y = static_cast<int>(pow(G, x)) % P;

        // Step 1: Alice chooses random r
        int r = rand() % (P - 1);
        int h = static_cast<int>(pow(G, r)) % P;

        // Step 2: Bob sends a random bit
        int b = rand() % 2;

        // Step 3: Alice sends s
        int s = (r + b * x) % (P - 1);

        // Step 4: Bob verifies
        int left = static_cast<int>(pow(G, s)) % P;
        int right = (h * static_cast<int>(pow(y, b)) % P) % P;

        return left == right;
    }
};

int main() {
    Blockchain myBlockchain;
    int choice;

    cout << "Welcome to the Asset Tokenization Blockchain\n";

    while (true) {
        cout << "\nSelect an option:\n";
        cout << "1. Add a Transaction\n";
        cout << "2. Mine Transactions\n";
        cout << "3. View Transactions for an Asset\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            // Take user input for a new transaction
            Transaction tx;
            cout << "Enter sender's name: ";
            cin >> tx.sender;
            cout << "Enter receiver's name: ";
            cin >> tx.receiver;
            cout << "Enter fractional ownership percentage (e.g., 25.0): ";
            cin >> tx.fractional_ownership;
            cout << "Enter asset ID: ";
            cin >> tx.assetID;
            cout << "Enter sensitive data (integer) for ZKP verification: ";
            cin >> tx.sensitive_data;

            myBlockchain.addTransaction(tx);
            cout << "Transaction added for verification.\n";

        } else if (choice == 2) {
            // Mine all pending transactions
            cout << "Mining pending transactions...\n";
            myBlockchain.minePendingTransactions();
            cout << "Transactions mined and added to the blockchain.\n";

        } else if (choice == 3) {
            // View transactions for a specific asset
            string assetID;
            cout << "Enter asset ID to view transactions: ";
            cin >> assetID;

            myBlockchain.viewAssetTransactions(assetID);

        } else if (choice == 4) {
            // Exit
            cout << "Exiting...\n";
            break;

        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}

