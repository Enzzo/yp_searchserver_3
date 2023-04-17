#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>

int MAX_RESULT_DOCUMENT_COUNT = 5;

using namespace std::literals;

struct Document{
    int id;
    int relevance;
};

bool HasDocumentGreaterRelevance(const Document& l, const Document& r){
    return l.relevance > r.relevance;
}

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

//  ------------------------------------------------------------------------------------
//  class SearchServer
//  ------------------------------------------------------------------------------------

class SearchServer{
private:
    struct DocumentContent{
        int id;
        std::vector<std::string> words;
    };

private:
    std::vector<DocumentContent> documents_;
    std::set<std::string> stop_words_;

public:
    void AddDocument(   int document_id, const std::string& document);
    void SetStopWords(const std::string& words);

private:
    std::vector<std::string> SplitIntoWordsNoStop(const std::string&);    
};

//  ------------------------------------------------------------------------------------
//  class SearchServer implimentation
//  ------------------------------------------------------------------------------------

//  AddDocument
void SearchServer::AddDocument( int document_id, 
                                const std::string& document){

    const std::vector<std::string> words = SplitIntoWordsNoStop(document);
    documents_.push_back({document_id, words});
}

//  SplitIntoWordsNoStop
std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& document){
    std::vector<std::string> words;

    for(const std::string& word : SplitIntoWords(document)){
        if(!stop_words_.count(word)){
            words.push_back(word);
        }
    }
    return words;
}

//  SetStopWords
void SearchServer::SetStopWords(const std::string& raw_words){
    for(const std::string& word : SplitIntoWords(raw_words)){
        stop_words_.insert(word);
    }
}
//  ------------------------------------------------------------------------------------
//  /SearchServer
//  ------------------------------------------------------------------------------------

// Разбирает text на слова и возвращает только те из них, которые не входят в stop_words
// std::set<std::string> ParseQuery(const std::string& text, const std::set<std::string>& stop_words) {
//     std::vector<std::string> words = SplitIntoWordsNoStop(text, stop_words);
//     return {words.begin(), words.end()};
// }

// int MatchDocument(  const DocumentContent& content, 
//                     const std::set<std::string>& query_words) {
//     int relevance = 0;
//     for(const std::string& word : content.words){
//         if(query_words.find(word) != query_words.end()){
//             relevance++;;
//         }
//     }
//     return relevance;
// }

// Для каждого документа возвращает пару {релевантность, id}
// std::vector<Document> FindAllDocuments(
//             const std::vector<DocumentContent>& documents,
//             const std::set<std::string>& query) {

//     std::vector<Document> matched_documents;

//     for(const DocumentContent& docs : documents){
//         int relevance = MatchDocument(docs, query);
//         if(0 < relevance){
//             matched_documents.push_back({docs.id, relevance});
//         }
//     }
//     return matched_documents;
// }

// Возвращает самые релевантные документы в виде вектора пар {id, релевантность}
// Находит не более MAX_RESULT_DOCUMENT_COUNT
// std::vector<Document> FindTopDocuments(
//     const std::vector<DocumentContent>& documents,
//     const std::set<std::string>& stop_words, const std::string& raw_query) {
    
//     std::set<std::string> query = ParseQuery(raw_query, stop_words);
//     std::vector<Document> docs = FindAllDocuments(documents, query);
    
//     std::sort(docs.begin(), docs.end(), HasDocumentGreaterRelevance);

//     if(MAX_RESULT_DOCUMENT_COUNT < docs.size()){
//         docs.resize(MAX_RESULT_DOCUMENT_COUNT);
//     }

//     // id relevance
//     return docs;
// }

int main() {
    SearchServer server;

    const std::string stop_words_joined = ReadLine();
    server.SetStopWords(stop_words_joined);

    // Считываем документы
    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        server.AddDocument(document_id, ReadLine());
    }

    const std::string query = ReadLine();
    //  Выводим результаты поиска по запросу query
    for (auto [document_id, relevance] : FindTopDocuments(documents, stop_words, query)) {
        std::cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s
             << std::endl;
    }
}