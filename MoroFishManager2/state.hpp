#ifndef __STATE_HEADER__
#define __STATE_HEADER__

namespace state {

enum struct FeedMode {
  LED_ONLY,
  LED_AND_FEED
};

typedef struct {
  FeedMode feed_mode;
  unsigned long timestamp;
} History;

void setup();

bool begin_feed(FeedMode feed_mode);

void set_feed_end();

bool is_feed_active();
FeedMode get_feed_mode();

int get_history_count();
History get_history(int i);

}

#endif /* __STATE_HEADER__ */