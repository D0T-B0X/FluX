#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

typedef unsigned int uint;

#include <queue>
#include "scene.h"

inline constexpr unsigned int FPS = 60;

enum EventType {
    CHANGE_VISCOSITY,
    CHANGE_RADIUS,
    CHANGE_SUBDIVISIONS,
    CHANGE_GRAVITY
};

struct Event {
    uint             uid;
    uint             priority;
    uint             activationFrame;
    float            changeVal;
    EventType        eType;

    bool operator<(const Event& other) const {
        return priority < other.priority;
    }
};

class EventHandler {
public:
    EventHandler(Scene& scene);

    void    handleEvents(uint currentFrame);

    // ---------- Event management -----------
    void    addEvent(const Event& event);
    void    removeEvent(const uint uid);   
    
    // ---------- Event Type handling -----------
    void    changeFluidVicosity(float viscosity);
    void    changeSphereRadius(float radius);
    void    changeSphereSubdivisions(float subdivs);
    void    changeGlobalGravity(float gravForce);

private:   
    std::priority_queue<Event>  eventQueue;
    std::priority_queue<uint>   cancelledEvents;

    Scene&                      eventScene;
};

#endif