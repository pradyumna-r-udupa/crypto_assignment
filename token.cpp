#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <random>
#include <cmath>
#include <unordered_map>

using namespace std;

const int P = 11; // Prime number
const int G = 2;  // Generator for P
const int DIFFICULTY = 2; // Difficulty for mining (number of leading zeros)

// Struct to represent a transaction
struct Transaction {
    string sender;
    string receiver;
    double amount;
    int sensitive_data; // Secret data to verify in ZKP
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

    void viewUser(string user) const {
        cout << "Transactions for " << user << ":\n";
        for (const Block &block : chain) {
            for (const Transaction &tx : block.transactions) {
                if (tx.sender == user || tx.receiver == user) {
                    cout << "From: " << tx.sender << ", To: " << tx.receiver
                         << ", Amount: " << tx.amount << endl;
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
        // Use Zero-Knowledge Proof for sensitive data verification
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

    // Create transactions with sensitive data (secret)
    Transaction tx1 = {"Alice", "Bob", 50.0, 4}; // sensitive_data = 4
    Transaction tx2 = {"Bob", "Charlie", 30.0, 3}; // sensitive_data = 3

    myBlockchain.addTransaction(tx1);
    myBlockchain.addTransaction(tx2);

    myBlockchain.minePendingTransactions();

    // View transactions for a user
    myBlockchain.viewUser("Alice");
    myBlockchain.viewUser("Bob");

    return 0;
}
