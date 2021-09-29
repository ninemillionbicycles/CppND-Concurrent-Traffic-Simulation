#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum class TrafficLightPhase {
  red,
  green,
};

template <class T>
class MessageQueue
{
public:
    void send(T &&msg);
    T receive();

private:
    std::deque<T> _queue;
    std::condition_variable _cond;
    std::mutex _mutex;
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / destructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    void toggleTrafficLightPhase();
    
private:
    // typical behaviour methods
    void cycleThroughPhases();

    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _queue;
};

#endif