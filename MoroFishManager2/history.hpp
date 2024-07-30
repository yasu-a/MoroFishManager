#ifndef __HISTORY_HEADER__
#define __HISTORY_HEADER__

#include "domain.hpp"

namespace history {

using namespace domain;

namespace feed {

int count();
void get(int i, FeedHistoryEntry &obj);
void add(FeedMode mode, unsigned long started_at, unsigned long ended_at);
void clear();

}

void setup();

}

#endif /* __HISTORY_HEADER__ */