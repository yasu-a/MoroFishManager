#ifndef __STATE_HEADER__
#define __STATE_HEADER__

#include "domain.hpp"

namespace state {

using namespace domain;

namespace feed {
bool begin(FeedMode feed_mode);
bool is_active();
FeedMode get_mode();
void end();
};

void setup();

}

#endif /* __STATE_HEADER__ */