#include <iostream>
#include <random>
#include <chrono>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // perform modification of _queue under the lock
    std::unique_lock<std::mutex> myLock(_mutex);

    // pass unique lock to condition variable
    // check that new data is really available via !_queue.empty() to avoid issuing wait() in case of spurious wake up
    _cond.wait(myLock, [this] { return !_queue.empty(); });

    // get last element from _queue
    T msg = std::move(_queue.back());
    // _queue.pop_back(); // does not work because if traffic light gets toggled multiple times without a vehicle 
    // visiting the intersection, messages pile up in the queue and thus the predicate used in wait() does not work 
    // anymore for a spurious wake-up 
    _queue.clear();

    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // perform modification of _messages under the lock
    std::lock_guard<std::mutex> myLock(_mutex);

    // add msg to queue
    _queue.push_back(std::move(msg));

    // notify client after pushing new message into deque
    _cond.notify_one(); 
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

// Blocks during red traffic light and returns once traffic light has switched to green
void TrafficLight::waitForGreen()
{
    while (true)
    {
        TrafficLightPhase phase = _queue.receive();

        if (phase == TrafficLightPhase::green)
        {
            break;
        }
    }
}

// Starts cycleThroughPhases“() as a thread and adds the thread to the thread queue
void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
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
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float> diff; // time between start and end in seconds
    float target  = randomFloatBetween(4.0, 6.0); // set random target cycle duration between 4 to 6 seconds

    while (true) {
        diff = end - start;

        if (diff.count() >= target) // target cycle duration has been reached
        {
            // toggle traffic light status
            toggleTrafficLightPhase();

            // send update message to queue using move semantics
            _queue.send(std::move(getCurrentPhase()));
            
            // reset start time
            start = std::chrono::system_clock::now();

            // set new random target cycle duration
            target  = randomFloatBetween(4.0, 6.0);
        }

        end = std::chrono::system_clock::now();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}