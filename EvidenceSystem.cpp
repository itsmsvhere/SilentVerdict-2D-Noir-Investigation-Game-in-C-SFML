#include "EvidenceSystem.h"

EvidenceSystem::EvidenceSystem() { init(); }

void EvidenceSystem::init() {
    list.clear();
    list.push_back({"Knife recovered",      "A blood-stained knife found near the creek."});
    list.push_back({"Bootprints detected",  "Heavy boot marks leading deep into the forest."});
    list.push_back({"Phone logs retrieved", "Last call made at 11:47 PM to an unknown number."});
}

const std::vector<Evidence>& EvidenceSystem::getList() const { return list; }
void EvidenceSystem::reset() { init(); }
