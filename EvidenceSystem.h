#pragma once
#include <string>
#include <vector>

struct Evidence {
    std::string name;
    std::string description;
};

class EvidenceSystem {
public:
    EvidenceSystem();
    const std::vector<Evidence>& getList() const;
    void reset();
private:
    std::vector<Evidence> list;
    void init();
};
