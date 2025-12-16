#include "util/eventHandler.h"

EventHandler::EventHandler(Scene& scene) : eventScene(scene) { }

void EventHandler::handleEvents(uint currentFrame) {
    if (eventQueue.top().activationFrame < currentFrame) {
        eventQueue.pop();
        return;
    }

    if (eventQueue.top().uid == cancelledEvents.top()) {
        eventQueue.pop();
        cancelledEvents.pop();
        return;
    }

    if (currentFrame == eventQueue.top().activationFrame) {
        // TODO => Implement event handling
    }
}

void EventHandler::addEvent(const Event& e) {
    eventQueue.push(e);
}

void EventHandler::removeEvent(const uint uid) {
    cancelledEvents.push(uid);
}

void EventHandler::changeSphereRadius(float radius) {
}
