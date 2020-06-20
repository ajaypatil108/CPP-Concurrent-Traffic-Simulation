#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

//Declaring generic templatized MessageQueue class
template <class T>
class MessageQueue
{
public:
// public methods
MessageQueue(){};
T receive();
void send (T && msg);

private:
std::deque<T> _queue;
std::condition_variable _cond;
std::mutex _mutex;
};

// Declaring enums for traffic light states
 enum class TrafficLightPhase {red, green}; 

// Defining TraffigLight class
class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    // Declaring MessageQueue monitor object of type TrafficLightPhase
    MessageQueue<TrafficLightPhase> _monitorObj;
    TrafficLightPhase _currentPhase; 
};

#endif