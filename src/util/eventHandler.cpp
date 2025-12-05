#include "util/eventHandler.h"

void EventHandler::handleEvents(uint currentFrame) {
    if (eventQueue.top().uid == cancelledEvents.top()) {
        eventQueue.pop();
        cancelledEvents.pop();
    }

    if (currentFrame == eventQueue.top().activationFrame) {
        eventQueue.top().handler();
    }
}

void EventHandler::addEvent(const Event& e) {
    eventQueue.push(e);
}

void EventHandler::removeEvent(const uint uid) {
    cancelledEvents.push(uid);
}