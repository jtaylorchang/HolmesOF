#include "sleuth.hpp"

/**
 * Google Custom Search REST API keys to cycle through
 */
vector<SearchCred> search_credentials_ = {
    SearchCred("AIzaSyAMeo1WRPJsfDYR8fW389Zia7fl-6rVKvU",
               "011755180049813977756:txa6on3hysw"),
    SearchCred("AIzaSyCE2PlTlUggSUgydHxFQS8H8ViLJXoMG4M",
               "017993560926643083890:u9edc5uskmq"),
    SearchCred("AIzaSyAPJBozPxFNJUm7eIS2ZRv9GNMm2P_S0xo",
               "011027612069579013146:xbt_oj4htzq"),
    SearchCred("AIzaSyBYQcnGoPS1U_4gNZax8ha-XjuFEBIuv6o",
               "015228221608509576590:togix_mvjza"),
    SearchCred("AIzaSyBeLv2dwgH6UGnmVkeO6PkH0qqGn-Cjznw",
               "016956300416314494911:qmqbjx-nre4")
};

/**
 * Chooses a random set of credentials to use to distribute searches across multiple
 */
SearchCred &ChooseCredentials() {
    int index = rand() % search_credentials_.size();
    return search_credentials_[index];
}

/**
 * Receive the response from the urlResponse event
 */
void ReceiveResponse(ofHttpResponse &response, string question, vector<string> answers, vector<double> &confidences) {
    cout << "Received response in Sleuth [ASYNC]" << endl;
    
    ofxJSONElement json = LoadSearchResults(response.data.getText());
    ProcessBasic(json, question, answers, confidences);
}

/**
 * Get the JSON for Google CSE for the given query
 */
ofxJSONElement LoadSearchResults(string content) {
    ofxJSONElement json (content);
    return json;
}

/**
 * Strip the snippets from the given JSON data
 */
vector<string> StripSnippets(ofxJSONElement &json) {
    vector<string> snippets;
    
    std::size_t size = json["items"].size();
    for (Json::ArrayIndex i = 0; i < size; i++) {
        snippets.push_back(json["items"][i]["title"].asString());
        snippets.push_back(json["items"][i]["snippet"].asString());
    }
    
    return snippets;
}

int CountAnswerOccurrences(string source, string answer) {
    string lower_source = ToLowerCase(source);
    string lower_answer = ToLowerCase(answer);
    vector<string> pieces = Split(lower_answer, ' ');
    
    int count = 0;
    
    for (string &piece : pieces) {
        count += Count(lower_source, piece);
    }
    
    return count;
}

/**
 * Increase confidence levels using the default search
 */
void SearchBasic(SearchCred &cred, string question, vector<string> answers) {
    cout << "Requesting basic search [ASYNC]" << endl;
    
    ofLoadURLAsync(cred.GetUrl(question));
}

/**
 * Process the basic search JSON and update the confidence levels
 */
void ProcessBasic(ofxJSONElement &json, string question, vector<string> answers, vector<double> &confidences) {
    cout << "Processing basic search snippets:" << endl;
    
    vector<string> snippets = StripSnippets(json);
    
    int max_count = 0;
    
    for (string &snippet : snippets) {
        cout << snippet << endl;
        
        for (int i = 0; i < answers.size(); i++) {
            int count = CountAnswerOccurrences(snippet, answers[i]);
            confidences[i] += count;
            max_count += count;
        }
    }
    
    for (int i = 0; i < confidences.size(); i++) {
        confidences[i] /= max_count;
    }
}

/**
 * Start the JSON collection for the given question
 */
void Investigate(string question, vector<string> answers) {
    SearchCred &credentials = ChooseCredentials();
    
    SearchBasic(credentials, question, answers);
}
