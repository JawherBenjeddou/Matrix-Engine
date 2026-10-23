#pragma once
#include "Common.h"
///@file Event.h  Lightweight named-event bus (publish/subscribe by string key).

namespace Matrix::events
{
    using EventHandler = std::function<void()>;
    using HandlerId = uint64_t;

    /// Global event bus. Handlers are keyed by an event name and invoked synchronously on Broadcast().
    class MATRIX_DLL Event
    {
    public:
        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;

        static Event& GetInstance();

        /// Subscribes a handler to an event name and returns an id that can be used to unsubscribe.
        HandlerId AddFunction(const std::string& eventType, EventHandler func);

        /// Removes a previously registered handler. Returns true if one was removed.
        bool RemoveFunction(HandlerId id);

        /// Invokes every handler registered for the event name. Returns the number of handlers called.
        size_t Broadcast(const std::string& eventType);

        /// Number of handlers currently subscribed to an event name.
        size_t SubscriberCount(const std::string& eventType) const;

        /// Removes every handler.
        void Clear();

    private:
        Event() = default;

        struct Subscriber
        {
            HandlerId Id;
            EventHandler Func;
        };

        HandlerId m_NextId = 1;
        std::unordered_map<std::string, std::vector<Subscriber>> m_Subscribers;
    };
}

/// Binds a member function of `this` to an event type.
#define BIND_EVENT_FUNCTION(eventType, functionName) \
    ::Matrix::events::Event::GetInstance().AddFunction(eventType, std::bind(&functionName, this))

/// Broadcasts an event to all subscribers.
#define BROADCAST_EVENT(eventType) \
    ::Matrix::events::Event::GetInstance().Broadcast(eventType)
