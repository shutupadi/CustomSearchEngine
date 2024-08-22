#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <queue>
#include <sstream>
#include <cmath>
#include <algorithm>

// Node structure for Trie
struct TrieNode {
    std::unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
    std::unordered_set<int> documentIDs; // List of document IDs where the word appears

    TrieNode() : isEndOfWord(false) {}
};

// Trie class
class Trie {
private:
    TrieNode* root;

public:
    Trie() {
        root = new TrieNode();
    }

    void insert(const std::string& word, int documentID) {
        TrieNode* current = root;
        for (char ch : word) {
            if (current->children.find(ch) == current->children.end()) {
                current->children[ch] = new TrieNode();
            }
            current = current->children[ch];
        }
        current->isEndOfWord = true;
        current->documentIDs.insert(documentID);
    }

    std::unordered_set<int> search(const std::string& word) {
        TrieNode* current = root;
        for (char ch : word) {
            if (current->children.find(ch) == current->children.end()) {
                return {};
            }
            current = current->children[ch];
        }
        return current->isEndOfWord ? current->documentIDs : std::unordered_set<int>();
    }

    void autocomplete(TrieNode* node, std::string prefix, std::vector<std::string>& results) {
        if (node->isEndOfWord) {
            results.push_back(prefix);
        }
        for (auto& child : node->children) {
            autocomplete(child.second, prefix + child.first, results);
        }
    }

    std::vector<std::string> getAutocompleteSuggestions(const std::string& prefix) {
        TrieNode* current = root;
        for (char ch : prefix) {
            if (current->children.find(ch) == current->children.end()) {
                return {};
            }
            current = current->children[ch];
        }
        std::vector<std::string> results;
        autocomplete(current, prefix, results);
        return results;
    }
};

// Inverted Index class
class InvertedIndex {
private:
    std::unordered_map<std::string, std::unordered_map<int, int>> index;
    std::unordered_map<int, int> documentLengths;
    int totalDocuments;

public:
    InvertedIndex() : totalDocuments(0) {}

    void addDocument(int documentID, const std::string& content) {
        std::istringstream iss(content);
        std::string word;
        int wordCount = 0;

        while (iss >> word) {
            index[word][documentID]++;
            wordCount++;
        }

        documentLengths[documentID] = wordCount;
        totalDocuments++;
    }

    std::unordered_map<int, double> searchWord(const std::string& word) {
        std::unordered_map<int, double> result;
        if (index.find(word) == index.end()) return result;

        int docFreq = index[word].size();
        double idf = log(static_cast<double>(totalDocuments) / docFreq);

        for (const auto& [docID, termFreq] : index[word]) {
            double tf = static_cast<double>(termFreq) / documentLengths[docID];
            result[docID] = tf * idf;
        }

        return result;
    }

    std::unordered_map<int, double> searchPhrase(const std::string& phrase) {
        std::istringstream iss(phrase);
        std::string word;
        std::unordered_map<int, double> combinedResults;
        bool firstWord = true;

        while (iss >> word) {
            std::unordered_map<int, double> wordResults = searchWord(word);

            if (firstWord) {
                combinedResults = wordResults;
                firstWord = false;
            } else {
                for (auto it = combinedResults.begin(); it != combinedResults.end();) {
                    if (wordResults.find(it->first) != wordResults.end()) {
                        it->second += wordResults[it->first];
                        ++it;
                    } else {
                        it = combinedResults.erase(it);
                    }
                }
            }
        }

        return combinedResults;
    }

    std::unordered_map<int, double> searchWithRanking(const std::string& query) {
        std::istringstream iss(query);
        std::string word;
        std::unordered_map<int, double> results;

        while (iss >> word) {
            auto wordResults = searchWord(word);
            for (const auto& [docID, score] : wordResults) {
                results[docID] += score;
            }
        }

        return results;
    }
};

// Simple Search Engine class
class SearchEngine {
private:
    Trie trie;
    InvertedIndex invertedIndex;
    std::unordered_map<int, std::string> documentStore;

public:
    void addDocument(int documentID, const std::string& content) {
        std::istringstream iss(content);
        std::string word;
        while (iss >> word) {
            trie.insert(word, documentID);
        }
        invertedIndex.addDocument(documentID, content);
        documentStore[documentID] = content;
    }

    void search(const std::string& query) {
        auto rankedResults = invertedIndex.searchWithRanking(query);
        if (rankedResults.empty()) {
            std::cout << "No results found for \"" << query << "\"." << std::endl;
            return;
        }

        std::vector<std::pair<int, double>> sortedResults(rankedResults.begin(), rankedResults.end());
        std::sort(sortedResults.begin(), sortedResults.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        std::cout << "Search results for \"" << query << "\":" << std::endl;
        for (const auto& [docID, score] : sortedResults) {
            std::cout << "Document ID: " << docID << " (Score: " << score << ")" << std::endl;
            displayDocument(docID);
        }
    }

    void searchPhrase(const std::string& phrase) {
        auto results = invertedIndex.searchPhrase(phrase);
        if (results.empty()) {
            std::cout << "No results found for \"" << phrase << "\"." << std::endl;
            return;
        }

        std::cout << "Search results for phrase \"" << phrase << "\":" << std::endl;
        for (const auto& [docID, score] : results) {
            std::cout << "Document ID: " << docID << " (Score: " << score << ")" << std::endl;
            displayDocument(docID);
        }
    }

    void displayDocument(int documentID) {
        if (documentStore.find(documentID) != documentStore.end()) {
            std::cout << "Document " << documentID << ": " << documentStore[documentID] << std::endl;
        } else {
            std::cout << "Document not found!" << std::endl;
        }
    }

    void autocomplete(const std::string& prefix) {
        auto suggestions = trie.getAutocompleteSuggestions(prefix);
        if (suggestions.empty()) {
            std::cout << "No autocomplete suggestions for \"" << prefix << "\"." << std::endl;
            return;
        }

        std::cout << "Autocomplete suggestions for \"" << prefix << "\":" << std::endl;
        for (const std::string& suggestion : suggestions) {
            std::cout << suggestion << std::endl;
        }
    }
};

// Test the Enhanced Search Engine
int main() {
    SearchEngine searchEngine;

    searchEngine.addDocument(1, "Hello world, this is a simple search engine.");
    searchEngine.addDocument(2, "Hello again, this search engine indexes documents.");
    searchEngine.addDocument(3, "The world is full of data, and this engine searches through it.");

    // Search for words
    searchEngine.search("Hello");
    searchEngine.search("world");
    searchEngine.search("engine");

    // Search for phrases
    searchEngine.searchPhrase("search engine");

    // Autocomplete suggestions
    searchEngine.autocomplete("sear");
    searchEngine.autocomplete("wor");

    return 0;
}
