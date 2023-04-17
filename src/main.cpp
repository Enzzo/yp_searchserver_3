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
    const std::vector<Document> FindTopDocuments(const std::string& raw_query) const;
    static int MatchDocument(const DocumentContent&, const std::set<std::string>&);

private:
    const std::vector<std::string> SplitIntoWordsNoStop(const std::string&) const;
    const std::vector<Document> FindAllDocuments(const std::set<std::string>&) const;
    const std::set<std::string> ParseQuery(const std::string&) const;
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
const std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& document)const{
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

//  FindAllDocuments
const std::vector<Document> SearchServer::FindAllDocuments(const std::set<std::string>& query) const {

    std::vector<Document> matched_documents;

    for(const DocumentContent& docs : documents_){
        int relevance = MatchDocument(docs, query);
        if(0 < relevance){
            matched_documents.push_back({docs.id, relevance});
        }
    }
     return matched_documents;
}

//  FindTopDocuments
const std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const {
    
    std::set<std::string> query = ParseQuery(raw_query);
    std::vector<Document> docs = FindAllDocuments(query);
    
    std::sort(docs.begin(), docs.end(), [](const Document& lhs, const Document& rhs){
        return lhs.relevance > rhs.relevance;
    });

    if(MAX_RESULT_DOCUMENT_COUNT < docs.size()){
        docs.resize(MAX_RESULT_DOCUMENT_COUNT);
    }

    // id relevance
    return docs;
}

//  ParseQuery
//  Разбирает text на слова и возвращает только те из них, которые не входят в stop_words
const std::set<std::string> SearchServer::ParseQuery(const std::string& text) const {
    std::vector<std::string> words = SplitIntoWordsNoStop(text);
    return {words.begin(), words.end()};
}

//  MatchDocument
int SearchServer::MatchDocument(const DocumentContent& content, const std::set<std::string>& query_words) {
    int relevance = 0;
    for(const std::string& word : content.words){
        if(query_words.find(word) != query_words.end()){
            relevance++;;
        }
    }
    return relevance;
}
//  ------------------------------------------------------------------------------------
//  /SearchServer
//  ------------------------------------------------------------------------------------



SearchServer CreateSearchServer();

int main() {
    SearchServer server = CreateSearchServer();

    const std::string query = ReadLine();
    //  Выводим результаты поиска по запросу query
    for (auto [document_id, relevance] : server.FindTopDocuments(query)) {
        std::cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s
             << std::endl;
    }
}

SearchServer CreateSearchServer(){
    SearchServer server;
    const std::string stop_words_joined = ReadLine();
    server.SetStopWords(stop_words_joined);

    // Считываем документы
    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        server.AddDocument(document_id, ReadLine());
    }

    return server;
}