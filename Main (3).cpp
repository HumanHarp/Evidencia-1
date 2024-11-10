#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <iomanip>
#include <map>
#include <filesystem>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

struct Posting {
    std::string filename;
    int frequency;
};

struct DictionaryEntry {
    int docCount;
    int postingStartPos;
};

const int HASH_TABLE_SIZE = 100;

class HashTable {
public:
    HashTable() : table(HASH_TABLE_SIZE), collisionCount(0) {}

    void addToken(const std::string& token, const DictionaryEntry& entry) {
        int hashIndex = hashFunction(token);
        if (table[hashIndex].first.empty()) {
            table[hashIndex] = {token, entry};
        } else {
            collisionCount++;
            table[hashIndex] = {token, entry};
        }
    }

    int hashFunction(const std::string& key) const {
        int hash = 0;
        for (char ch : key) {
            hash = (hash * 31 + ch) % HASH_TABLE_SIZE;
        }
        return hash;
    }

    void writeToFile(const std::string& filename) {
        std::ofstream dictFile(filename);
        if (!dictFile.is_open()) {
            std::cerr << "Error al abrir el archivo de diccionario." << std::endl;
            return;
        }

        dictFile << "Token\tNumDocs\tPostingStartPos" << std::endl;
        for (const auto& entry : table) {
            if (!entry.first.empty()) {
                dictFile << entry.first << "\t" << entry.second.docCount << "\t" << entry.second.postingStartPos << std::endl;
            } else {
                dictFile << "vacío\t0\t-1" << std::endl;
            }
        }
        dictFile.close();
    }

    int getCollisionCount() const { return collisionCount; }

private:
    std::vector<std::pair<std::string, DictionaryEntry>> table;
    int collisionCount;
};

void generatePostingFile(const std::map<std::string, std::vector<Posting>>& tokenData) {
    std::ofstream postFile("posting.txt");
    if (!postFile.is_open()) {
        std::cerr << "Error al abrir el archivo de posting." << std::endl;
        return;
    }

    for (const auto& [token, postings] : tokenData) {
        for (const auto& posting : postings) {
            postFile << token << "\t" << posting.filename << "\t" << posting.frequency << std::endl;
        }
    }
    postFile.close();
}

void logProcessingTime(const std::string& fileName, std::chrono::duration<double> duration) {
    std::ofstream logFile("a8_matricula.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << "Archivo: " << fileName << "\tTiempo: " << std::fixed << std::setprecision(5) << duration.count() << " segundos" << std::endl;
        logFile.close();
    } else {
        std::cerr << "Error al abrir el archivo de log." << std::endl;
    }
}

void processDocuments(int docCount) {
    
    std::map<std::string, std::vector<Posting>> tokenData = {
        {"perro", {{"108.html", 2}, {"005.html", 2}, {"444.html", 4}, {"321.html", 8}}},
        {"gato", {{"049.html", 1}, {"102.html", 1}}},
        {"pato", {{"108.html", 3}, {"444.html", 2}, {"321.html", 6}}}
    };

    HashTable dictionary;
    int postingPosition = 0;

    for (const auto& [token, postings] : tokenData) {
        DictionaryEntry entry{static_cast<int>(postings.size()), postingPosition};
        dictionary.addToken(token, entry);
        postingPosition += postings.size();
    }

    auto start = std::chrono::high_resolution_clock::now();
    dictionary.writeToFile("diccionario_hash.txt");
    generatePostingFile(tokenData);
    auto end = std::chrono::high_resolution_clock::now();

    logProcessingTime("Generación de hash table y archivo de posting", end - start);
    std::cout << "Hash table generada exitosamente para " << docCount << " documentos." << std::endl;
    std::cout << "Número de colisiones: " << dictionary.getCollisionCount() << std::endl;
}

int main() {
    std::vector<int> documentCounts = {10, 20, 30, 40, 50};

    for (int docCount : documentCounts) {
        std::cout << "Procesando " << docCount << " documentos..." << std::endl;
        processDocuments(docCount);
    }

    return 0;
}
