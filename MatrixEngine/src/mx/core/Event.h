#pragma once
#include "Common.h"
#include "pch.h"

namespace Matrix
{
	namespace events  
	{
       
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

            void AddFunction(const std::string& eventtype, std::function<void()> func);

            void Broadcast(const std::string& eventtype);

        private:
            // Private constructor to enforce singleton pattern.
            Event() {}
            std::unordered_map<std::string, std::vector<std::function<void()>>> m_Subscribers;
        };

	}
}

/**
    * @brief macro for adding functions to the event
    * @param eventType: A string representing the type of the event.
    * @param functionName: The member function to be bound and added as a callback.
*/
#define BIND_EVENT_FUNCTION(eventType, functionName) \
    Matrix::events::Event::GetInstance().AddFunction(eventType, std::bind(&functionName, this))


/**
    * @brief macro for brodcasting functions that exists in the given event
    * @param eventType: A string representing the type of the event.
*/
#define BROADCAST_EVENT(eventType) \
    Matrix::events::Event::GetInstance().Broadcast(eventType)
