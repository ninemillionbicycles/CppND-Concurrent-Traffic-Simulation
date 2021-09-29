#include <iostream>
#include <random>
#include <chrono>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
}

// Returns a random float number in the range from [n,m)
double randomFloatBetween(float n, float m)
{
    return m + (rand() / (RAND_MAX / (m-n)));
}

/* Implementation of class "TrafficLight" */

// Constructor
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

// TODO write some text
void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

// Starts cycleThroughPhases“() as a thread and adds the thread to the thread queue
void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this)); // TODO: Check!!!
}

// Return current phase of traffic light
TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

// Toggles traffic light between red and green
void TrafficLight::toggleTrafficLightPhase()
{
    if (getCurrentPhase() == TrafficLightPhase::green)
    {
        _currentPhase = TrafficLightPhase::red;
    }
    else
    {
        _currentPhase = TrafficLightPhase::green;
    }
}

// Virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float> diff; // time between start and end in seconds
    float target  = randomFloatBetween(4.0, 6.0); // set random target cycle duration between 4 to 6 seconds

    while(true) {
        diff = end - start;

        if (diff.count() >= target) // target cycle duration has been reached
        {
            // toggle traffic light status
            toggleTrafficLightPhase();

            // send update message to queue using move semantics
            _queue.send(std::move(getCurrentPhase())); // TODO: Check!!!
            
            // reset start time
            start = std::chrono::system_clock::now();

            // set new random target cycle duration
            target  = randomFloatBetween(4.0, 6.0);
        }

        end = std::chrono::system_clock::now();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}