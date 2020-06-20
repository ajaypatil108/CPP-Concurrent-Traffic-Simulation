#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <mutex>
#include <thread>
#include <chrono>
#include "TrafficLight.h"

 
template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this]{return !_queue.empty();}); 
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}


template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> uLock(_mutex);
    _queue.push_back(std::move(msg));
    _cond.notify_one();
}
 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // returns when traffic light turns to green
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (_monitorObj.receive() == TrafficLightPhase::green){
            return;
            }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // starting a thread with cycleTroughPhases method 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this)); 
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // randomizer for traffic light time
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist (4000, 6000);
    int duration = dist(gen);
    
    std::chrono::time_point<std::chrono::system_clock> lastUpdateOnTL;

    // init stop watch
    lastUpdateOnTL = std::chrono::system_clock::now();

    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        int elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdateOnTL).count();
        if (elapsedTime >= duration)
        {
            if(_currentPhase == TrafficLightPhase::red)
            {
                _currentPhase = TrafficLightPhase::green;
            }
            else
            {
                _currentPhase = TrafficLightPhase::red;
            }

            TrafficLightPhase message = TrafficLight::getCurrentPhase();
            _monitorObj.send(std::move(message)); 
            duration = dist(gen);
            lastUpdateOnTL = std::chrono::system_clock::now();
        } 
    }
}
