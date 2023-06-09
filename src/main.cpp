#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <fstream>

int MAX_RESULT_DOCUMENT_COUNT = 5;

using namespace std;

struct Query{
    std::set<std::string> plus_words;
    std::set<std::string> minus_words;
};

struct Document{
    int id;
    double relevance;
};

std::string ReadLine(std::istream& ist = std::cin) {
    std::string s;
    std::getline(ist, s);
    return s;
}

int ReadLineWithNumber(std::istream& ist = std::cin) {
    int result = 0;
    ist >> result;
    ReadLine(ist);
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

private:
    int document_count_ = 0;
    std::map<std::string, std::map<int, double>> word_to_document_freqs_;
    std::set<std::string> stop_words_;

public:
    void AddDocument(   int document_id, const std::string& document);
    void SetStopWords(const std::string& words);
    const std::vector<Document> FindTopDocuments(const std::string& raw_query) const;
    static int MatchDocument(const std::set<std::string>&, const Query&);

private:
    const std::vector<std::string> SplitIntoWordsNoStop(const std::string&) const;
    const std::vector<Document> FindAllDocuments(const Query&) const;
    const Query ParseQuery(const std::string&) const;
};

//  ------------------------------------------------------------------------------------
//  class SearchServer implimentation
//  ------------------------------------------------------------------------------------

//  AddDocument
void SearchServer::AddDocument( int document_id, 
                                const std::string& document){

    for(const std::string word : SplitIntoWordsNoStop(document)){
        word_to_documents_[word].insert(document_id);
    }
}

//  SplitIntoWordsNoStop
const std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& document) const {
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

//  FindTopDocuments
const std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const {
    
    Query query = ParseQuery(raw_query);
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

//  FindAllDocuments
const std::vector<Document> SearchServer::FindAllDocuments(const Query& query) const {

    std::vector<Document> matched_documents;
    std::map<int, int> document_to_relevance;

    //  инвертируем список документов. Идентификатор - ключ
    std::map<int, std::set<std::string>> documents;
    for(const auto& [word, ids] : word_to_documents_){
        for(const int id : ids){
            documents[id].emplace(word);
        }
    }

    for(const auto& [id, words] : documents){
        int relevance = MatchDocument(words, query);
        if(0 < relevance){
            matched_documents.push_back({id, relevance});
        }
    }
     return matched_documents;
}

//  MatchDocument
int SearchServer::MatchDocument(const std::set<std::string>& content, const Query& query_words) {

    if(query_words.plus_words.empty()) return 0;

    int relevance = 0;
    
    for(const std::string& word : content){
        if(query_words.minus_words.count(word) != 0) return 0;
        if(query_words.plus_words.count(word) != 0) relevance++;        
    }
    
    return relevance;
}


//  ParseQuery
//  Разбирает text на слова и возвращает только те из них, которые не входят в stop_words
const Query SearchServer::ParseQuery(const std::string& text) const {
    Query query;
    for(const std::string& word : SplitIntoWordsNoStop(text)){
        if(word[0] == '-'){
            query.minus_words.insert(word.substr(1));
        }
        else{
            query.plus_words.insert(word);
        }
    }
    return query;
}

//  ------------------------------------------------------------------------------------
//  /SearchServer
//  ------------------------------------------------------------------------------------

SearchServer CreateSearchServer(std::istream& ist = std::cin);

int main() {
    std::ifstream ist("input.txt");
    //  std::istream& ist = std::cin;
    setlocale(LC_ALL, "russian");

    SearchServer server = CreateSearchServer(ist);

    const std::string query = ReadLine(ist);
    //  Выводим результаты поиска по запросу query
    for (auto [document_id, relevance] : server.FindTopDocuments(query)) {
        std::cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s
             << std::endl;
    }
}

SearchServer CreateSearchServer(std::istream& ist){
    SearchServer server;
    const std::string stop_words_joined = ReadLine(ist);
    server.SetStopWords(stop_words_joined);

    // Считываем документы
    const int document_count = ReadLineWithNumber(ist);
    for (int document_id = 0; document_id < document_count; ++document_id) {
        server.AddDocument(document_id, ReadLine(ist));
    }

    return server;
}