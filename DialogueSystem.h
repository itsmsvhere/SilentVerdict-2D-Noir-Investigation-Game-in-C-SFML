#pragma once
#include <string>
#include <vector>
#include <map>

// Must be inline (C++17) to avoid multiple-definition errors across TUs
inline const std::vector<std::string> QUESTIONS = {
    "Where were you last night?",
    "Did you visit the forest?",
    "Do you know the colleague?",
    "When did you last see her?",
    "Why were you near the forest?"
};

inline const std::vector<std::string> SUSPECTS = {
    "Husband", "Chef", "Colleague", "Suspect: Suicide"
};

class DialogueSystem {
public:
    DialogueSystem();
    std::string getResponse(int si, int qi) const;
    void applyQuestion(int si, int qi, std::map<std::string,int>& suspicion) const;
    void reset();
private:
    // responses[suspect][question]
    std::vector<std::vector<std::string>> resp;
    void init();
};
