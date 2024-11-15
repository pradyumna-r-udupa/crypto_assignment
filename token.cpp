#include<bits/stdc++.h>
using namespace std;

class Wallet {
public:
    unordered_map<string, double> assets; // Asset ID -> Fractional ownership


    void addOwnership(const string &assetID, double amount) {
        if (assets.find(assetID) == assets.end()) {
            assets[assetID] = 0;  // Initialize to 0% ownership if asset is new
        }
        if (assets[assetID] + amount <= 100.0) {
            assets[assetID] += amount;
        } else {
            cout << "Error: Adding " << amount << "% would exceed 100% ownership for asset " << assetID << ".\n";
        }
    }

    // Deducts ownership; initializes to 1000 if asset does not exist
    bool deductOwnership(const string &assetID, double amount) {
        if (assets.find(assetID) == assets.end()) {
            assets[assetID] = 100.0;  // Set default value for new assets
        }
        if (assets[assetID] >= amount) {
            assets[assetID] -= amount;
            return true;
        }
        return false; // Insufficient ownership
    }

    // Retrieves ownership; initializes to 1000 if asset does not exist
    double getOwnership(const string &assetID) {
        if (assets.find(assetID) == assets.end()) {
            assets[assetID] = 0; // Set default value for new assets
        }
        return assets[assetID];
    }

    // View all assets in the wallet
    void viewAssets() const {
        cout << "Assets in Wallet:\n";
        for (const auto &entry : assets) {
            cout << "Asset ID: " << entry.first << ", Ownership: " << entry.second << "%\n";
        }
    }
};


const int P = 11; // Prime number for ZKP
const int G = 2;  // Generator for ZKP
const int DIFFICULTY = 0; // Difficulty for mining (number of leading zeros)

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

class Blockchain {
public:
    Blockchain() {
        // Create genesis block
        chain.push_back(createBlock(vector<Transaction>(), "0"));
    }

   void addTransaction(Transaction tx) {
        // Verify transaction using Zero-Knowledge Proof
        if (verifyTransaction(tx)) {
            // Initialize ownership for sender and receiver if asset is new
            if (wallets[tx.sender].getOwnership(tx.assetID) == 0.0 && wallets[tx.receiver].getOwnership(tx.assetID) == 0.0) {
                wallets[tx.sender].assets[tx.assetID] = 100.0; // Sender owns 100% of new assets by default
            }

            double senderOwnership = wallets[tx.sender].getOwnership(tx.assetID);
            double receiverOwnership = wallets[tx.receiver].getOwnership(tx.assetID);

            // Check if sender has enough ownership
            if (senderOwnership >= tx.fractional_ownership) {
                // Check if receiver's ownership will exceed 100%
                if (receiverOwnership + tx.fractional_ownership <= 100.0) {
                    // Process the transaction
                    wallets[tx.sender].deductOwnership(tx.assetID, tx.fractional_ownership);
                    wallets[tx.receiver].addOwnership(tx.assetID, tx.fractional_ownership);
                    pendingTransactions.push_back(tx);
                    cout << "Transaction successfully added for verification.\n";
                } else {
                    cout << "Transaction failed: Receiver's ownership would exceed 100%.\n";
                }
            } else {
                cout << "Transaction failed: Insufficient ownership in sender's wallet.\n";
            }
        } else {
            cout << "Transaction failed verification!\n";
        }
    }

    void minePendingTransactions() {
        Block newBlock(chain.size(), pendingTransactions, chain.back().hash);
        newBlock.mineBlock();
        chain.push_back(newBlock);
        pendingTransactions.clear();
    }

    void viewAssetTransactions(const string &assetID) const {
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

    void viewUserWallet(const string &user) const {
        auto it = wallets.find(user);
        if (it != wallets.end()) {
            cout << "Wallet for user: " << user << endl;
            it->second.viewAssets();
        } else {
            cout << "No wallet found for user: " << user << endl;
        }
    }

private:
    vector<Block> chain;
    vector<Transaction> pendingTransactions;
    unordered_map<string, Wallet> wallets; // Mapping of username to Wallet

    Block createBlock(vector<Transaction> txs, const string &prevHash) {
        return Block(chain.size(), txs, prevHash);
    }

    bool verifyTransaction(Transaction tx) {
        // Zero-Knowledge Proof verification as defined earlier
        int x = tx.sensitive_data;
        int y = static_cast<int>(pow(G, x)) % P;

        int r = rand() % (P - 1);
        int h = static_cast<int>(pow(G, r)) % P;

        int b = rand() % 2;
        int s = (r + b * x) % (P - 1);

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
        cout << "4. View User Wallet\n";
        cout << "5. Exit\n";
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
            // View a specific user's wallet
            string user;
            cout << "Enter username to view wallet: ";
            cin >> user;
            myBlockchain.viewUserWallet(user);

        } else if (choice == 5) {
            // Exit
            cout << "Exiting...\n";
            break;

        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
