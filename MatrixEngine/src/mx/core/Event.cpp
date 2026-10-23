#include "pch.h"
#include "Event.h"

namespace Matrix::events
{
    Event& Event::GetInstance()
    {
        static Event instance;
        return instance;
    }

    HandlerId Event::AddFunction(const std::string& eventType, EventHandler func)
    {
        HandlerId id = m_NextId++;
        m_Subscribers[eventType].push_back({ id, std::move(func) });
        return id;
    }

    bool Event::RemoveFunction(HandlerId id)
    {
        for (auto it = m_Subscribers.begin(); it != m_Subscribers.end(); ++it)
        {
            auto& list = it->second;
            auto found = std::find_if(list.begin(), list.end(), [id](const Subscriber& s) { return s.Id == id; });
            if (found != list.end())
            {
                list.erase(found);
                if (list.empty())
                    m_Subscribers.erase(it);
                return true;
            }
        }
        return false;
    }

    size_t Event::Broadcast(const std::string& eventType)
    {
        auto it = m_Subscribers.find(eventType);
        if (it == m_Subscribers.end())
            return 0;

        // Copy so handlers may safely subscribe/unsubscribe while being invoked.
        std::vector<Subscriber> subscribers = it->second;
        for (const auto& subscriber : subscribers)
            subscriber.Func();
        return subscribers.size();
    }

    size_t Event::SubscriberCount(const std::string& eventType) const
    {
        auto it = m_Subscribers.find(eventType);
        return it == m_Subscribers.end() ? 0 : it->second.size();
    }

    void Event::Clear()
    {
        m_Subscribers.clear();
    }
}
