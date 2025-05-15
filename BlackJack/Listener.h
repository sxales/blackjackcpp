#pragma once

#include <vector>

// Listener interface
class Listener {
public:
    virtual void notify() = 0; // Pure virtual function
};

// Subject class that maintains a list of listeners
class Subject {
private:
    std::vector<Listener*> listeners; // List of listeners

public:
    void addListener(Listener* listener) {
        listeners.push_back(listener);
    }

    void removeListener(Listener* listener) {
        listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
    }

    void notifyListeners() {
        for (Listener* listener : listeners) {
            listener->notify(); // Call the notify function on each listener
        }
    }
};