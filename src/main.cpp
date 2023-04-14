#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>

using namespace std::literals;

std::string ReadLine() {
    std::string s;
    std::getline(std::cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    std::cin >> result;
    ReadLine();
    return result;
}

std::vector<std::string> SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

std::set<std::string> ParseStopWords(const std::string& text) {
    std::set<std::string> stop_words;
    for (const std::string& word : SplitIntoWords(text)) {
        stop_words.insert(word);
    }
    return stop_words;
}

std::vector<std::string> SplitIntoWordsNoStop(const std::string& text, const std::set<std::string>& stop_words) {
    std::vector<std::string> words;
    // проходим по всем словам из текста и проверяем, что их нет в списке стоп-слов
    for (const std::string& word : SplitIntoWords(text)) {
        if (stop_words.count(word) == 0) {
            words.push_back(word);
        }
    }
    // вернём результат без стоп-слов
    return words;
}

void AddDocument(   std::vector<std::pair<int, std::vector<std::string>>>& documents, 
                    const std::set<std::string>& stop_words,
                    int document_id,
                    const std::string& document) {
       
    documents.push_back(std::pair<int, std::vector<std::document_id, SplitIntoWordsNoStop(document, stop_words));
}

// Разбирает text на слова и возвращает только те из них, которые не входят в stop_words
std::set<std::string> ParseQuery(const std::string& text, const std::set<std::string>& stop_words) {
    std::vector<std::string> words = SplitIntoWordsNoStop(text, stop_words);
    return {words.begin(), words.end()};
}

// Возвращает true, если среди слов документа (document_words)
// встречаются слова поискового запроса query_words
int MatchDocument(  std::pair<int, const std::vector<std::string>>& content, 
                    const std::set<std::string>& query_words) {
    for(const std::string& word : document_words){
        if(query_words.find(word) != query_words.end()){
            return true;
        }
    }
    return false;
}

// Возвращает массив id документов, подходящих под запрос query
// Стоп-слова исключаются из поиска
std::vector<std::pair<int, int>> FindDocuments(const std::vector<std::pair<int, std::vector<std::string>>>& documents, 
            const std::set<std::string>& stop_words,
            const std::string& raw_query) {
    std::vector<int> matched_documents;

    std::set<std::string> query = ParseQuery(raw_query, stop_words);

    for(size_t i = 0; i < documents.size(); ++i){
        if(MatchDocument(documents[i], query)){
            matched_documents.push_back(i);
        }
    }

    // Напишите код функции
    // Воспользуйте вспомогательными функциями ParseQuery, MatchDocument
    // В качестве id документа используйте его индекс в массиве documents

    return matched_documents;
}

int main() {
    const std::string stop_words_joined = ReadLine();
    const std::set<std::string> stop_words = ParseStopWords(stop_words_joined);

    // Считываем документы
    std::vector<std::pair<int, std::vector<std::string>>> documents;
    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        AddDocument(documents, stop_words, document_id, ReadLine());
    }

    const std::string query = ReadLine();
    // Выводим результаты поиска по запросу query
    for (auto [document_id, relevance] : FindDocuments(documents, stop_words, query)) {
        std::cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s
             << std::endl;
    }
}