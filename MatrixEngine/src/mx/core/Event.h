#pragma once
#include "Common.h"
#include "pch.h"
#include "core/Logging.h"

namespace Matrix
{
	namespace events  
	{
        //id's for events
        enum class EventType
        {
            NONE = 0,
            WINDOWEVENTS,
            KEYEVENTS,
            MOUSEEVENTS,
            MOVE
        };
        // Singleton Event class
        class MATRIX_DLL Event {

        public:
            // Deleted copy constructor and assignment operator to prevent cloning.
            Event(const Event&) = delete;
            Event& operator=(const Event&) = delete;

            // Get the singleton instance.
            static Event& GetInstance() {
                static Event instance;
                return instance;
            }

            // Add a function and its event type.
            void AddFunction(EventType event, std::function<void()> func) {
                // Check if the event is already in the map
                auto it = m_Subscribers.find(event);
                if (it == m_Subscribers.end()) {
                    // Event not found, add a new entry with the function in a set
                    m_Subscribers[event] = { func };
                }
                else {
                    // Event found, insert the function into the existing set
                    it->second.push_back(func);
                }
                MX_CORE_WARN("Function added to the event list");
            }

            // Broadcast with no arguments 
            void Broadcast(EventType event) {
                auto it = m_Subscribers.find(event);
                if (it != m_Subscribers.end()) {
                    const auto& subscribers = it->second;
                    for (const auto& func : subscribers) {
                        func(); // Call with default-constructed Args
                    }
                }

                MX_CORE_WARN("Function is broadcasted");
            }
        private:
            // Private constructor to enforce singleton pattern.
            Event() {}
            std::unordered_map<EventType, std::vector<std::function<void()>>> m_Subscribers;
        };

	}
}
