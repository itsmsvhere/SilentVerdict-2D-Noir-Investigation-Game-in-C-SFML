#include "DialogueSystem.h"

DialogueSystem::DialogueSystem() { init(); }

void DialogueSystem::init() {
    resp.assign(4, std::vector<std::string>(5));

    // Husband (idx 0) — volatile, escalated a fight into something worse
    resp[0][0] = "I was home. I already said that. Stop asking.";
    resp[0][1] = "I... went for a walk. That area, yes. But I left.";
    resp[0][2] = "That colleague — he was always dragging her into late nights. I hated it.";
    resp[0][3] = "We fought. Badly. She said I was controlling. I followed her out.";
    resp[0][4] = "We argued near the forest path. Things got out of hand. I panicked and ran.";

    // Chef (idx 1) — calm, uninvolved
    resp[1][0] = "I was at the restaurant until midnight. Ask anyone.";
    resp[1][1] = "I have never been to that forest in my life.";
    resp[1][2] = "I saw her once at a company dinner. That is all.";
    resp[1][3] = "About two weeks ago, at the restaurant she visited.";
    resp[1][4] = "I was not near any forest. You have the wrong person.";

    // Colleague (idx 2) — nervous, aware of the marital tension
    resp[2][0] = "I was working late on a deadline. Logs will confirm it.";
    resp[2][1] = "We sometimes walked that trail for lunch. Nothing more.";
    resp[2][2] = "Yes, we were close colleagues. Her husband never liked that.";
    resp[2][3] = "She was upset that afternoon. Said her husband had been aggressive lately.";
    resp[2][4] = "She called me at 11 PM, crying. Said they had a bad fight. I told her to stay calm.";

    // Suicide (idx 3) — narrative construct
    resp[3][0] = "She had no alibi — she was alone and distressed.";
    resp[3][1] = "She went there herself. It was her thinking place.";
    resp[3][2] = "She had been under enormous stress from marital conflict.";
    resp[3][3] = "The fights were escalating. She may have reached a breaking point.";
    resp[3][4] = "She often walked the forest path alone when overwhelmed by conflict.";
}

std::string DialogueSystem::getResponse(int si, int qi) const {
    if (si < 0 || si >= 4 || qi < 0 || qi >= 5) return "(no response)";
    return resp[si][qi];
}

void DialogueSystem::applyQuestion(int si, int qi,
                                    std::map<std::string,int>& suspicion) const {
    const std::string& name = SUSPECTS[si];
    suspicion[name] += 5;

    if (si == 0) { // Husband accumulates highest
        if (qi == 1) suspicion[name] += 20; // admits being near forest
        if (qi == 3) suspicion[name] += 15; // admits following her
        if (qi == 4) suspicion[name] += 10; // admits things got out of hand
    }
    if (si == 2) { // Colleague moderate
        if (qi == 4) suspicion[name] += 10; // last call
        if (qi == 3) suspicion[name] += 5;  // knew about aggression
    }
    // Chef and Suicide remain low
}

void DialogueSystem::reset() { init(); }