#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>

int MAX_RESULT_DOCUMENT_COUNT = 5;

using namespace std::literals;

struct DocumentContent{
    int id;
    std::vector<std::string> words;
};

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

void AddDocument(   std::vector<DocumentContent>& documents, 
                    const std::set<std::string>& stop_words,
                    int document_id,
                    const std::string& document) {
       
    documents.push_back(DocumentContent{document_id, SplitIntoWordsNoStop(document, stop_words)});
}

// Разбирает text на слова и возвращает только те из них, которые не входят в stop_words
std::set<std::string> ParseQuery(const std::string& text, const std::set<std::string>& stop_words) {
    std::vector<std::string> words = SplitIntoWordsNoStop(text, stop_words);
    return {words.begin(), words.end()};
}

int MatchDocument(  const DocumentContent& content, 
                    const std::set<std::string>& query_words) {
    int relevance = 0;
    for(const std::string& word : content.words){
        if(query_words.find(word) != query_words.end()){
            relevance++;;
        }
    }
    return relevance;
}

// Для каждого документа возвращает пару {релевантность, id}
std::vector<std::pair<int, int>> FindAllDocuments(
            const std::vector<DocumentContent>& documents,
            const std::set<std::string>& query) {

    std::vector<std::pair<int, int>> matched_documents;

    for(const DocumentContent& docs : documents){
        int relevance = MatchDocument(docs, query);
        if(0 < relevance){
            matched_documents.push_back(std::pair(docs.id, relevance));
        }
    }
    return matched_documents;
}

// Возвращает самые релевантные документы в виде вектора пар {id, релевантность}
// Находит не более MAX_RESULT_DOCUMENT_COUNT
std::vector<std::pair<int, int>> FindTopDocuments(
    const std::vector<DocumentContent>& documents,
    const std::set<std::string>& stop_words, const std::string& raw_query) {
    
    std::set<std::string> query = ParseQuery(raw_query, stop_words);
    std::vector<std::pair<int, int>> docs = FindAllDocuments(documents, query);
    
    std::sort(docs.rbegin(), docs.rend());

    if(MAX_RESULT_DOCUMENT_COUNT < docs.size()){
        docs.resize(MAX_RESULT_DOCUMENT_COUNT);
    }

    // id relevance
    return docs;
}

int main() {
    const std::string stop_words_joined = ReadLine();
    const std::set<std::string> stop_words = ParseStopWords(stop_words_joined);

    // Считываем документы
    std::vector<DocumentContent> documents;
    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        AddDocument(documents, stop_words, document_id, ReadLine());
    }

    const std::string query = ReadLine();
    // Выводим результаты поиска по запросу query
    for (auto [document_id, relevance] : FindTopDocuments(documents, stop_words, query)) {
        std::cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s
             << std::endl;
    }
}