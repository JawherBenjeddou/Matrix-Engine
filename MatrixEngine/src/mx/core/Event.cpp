#include "pch.h"
#include "Event.h"

#include "core/Logging.h"

namespace Matrix
{
    namespace events
    {
            // Add a function and its event type.
            void Event::AddFunction(const std::string& eventtype, std::function<void()> func) {
                // Check if the event is already in the map
                auto it = m_Subscribers.find(eventtype);
                if (it == m_Subscribers.end()) {
                    // Event not found, add a new entry with the function in a set
                    m_Subscribers[eventtype] = {func};
                    MX_CORE_WARN("{} is not found and function will be added to the new event type", eventtype);
                }
                else {
                    // Event found, insert the function into the existing set
                    it->second.push_back(func);

                    MX_CORE_WARN("{} is found and function will be added",eventtype);
                }
            }

            // Broadcast with no arguments 
            void Event::Broadcast(const std::string& eventtype) {
                auto it = m_Subscribers.find(eventtype);
                if (it != m_Subscribers.end()) {
                    const auto& subscribers = it->second;
                    for (const auto& func : subscribers) {
                        func(); // Call with default-constructed Args

                        MX_CORE_WARN("{} events are being broadcasted",eventtype);
                    }
                }

            }
        

    }
}
