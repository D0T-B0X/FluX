#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

inline constexpr unsigned int FPS = 60;

#include <queue>

typedef void (*ActionCallBack)();
typedef unsigned int uint;

struct Event {
    uint             uid;
    uint             priority;
    uint             activationFrame;
    ActionCallBack   handler;
};

class EventHandler {
public:

    void    handleEvents(uint currentFrame);

    // ---------- Event management -----------
    void    addEvent(const Event& event);
    void    removeEvent(const uint uid);

private:   
    std::priority_queue<Event>  eventQueue;
    std::priority_queue<uint>   cancelledEvents;
};

#endif