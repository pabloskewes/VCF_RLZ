#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread> // Include this for std::this_thread::sleep_for

class Timer {
public:
    // Get the single instance of the Timer
    static Timer& getInstance() {
        static Timer instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent copies
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    void startEvent(const std::string& event_name) {
        std::lock_guard<std::mutex> lock(mtx);
        auto now = std::chrono::high_resolution_clock::now();
        event_start_times[event_name] = now;
    }

    void endEvent(const std::string& event_name) {
        std::lock_guard<std::mutex> lock(mtx);
        auto now = std::chrono::high_resolution_clock::now();
        auto start_it = event_start_times.find(event_name);
        if (start_it != event_start_times.end()) {
            std::chrono::duration<double> elapsed = now - start_it->second;
            events.emplace_back(elapsed.count(), event_name);
            event_start_times.erase(start_it);
        } else {
            std::cerr << "Event " << event_name << " was not started." << std::endl;
        }
    }

    void report() const {
        auto global_now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> global_elapsed = global_now - global_start;

        std::cout << "Global Time: " << global_elapsed.count() << " seconds\n";
        std::cout << "Event Report:\n";
        for (const auto& [time, message] : events) {
            std::cout << "Event: " << message << " - Time: " << time << " seconds\n";
        }
    }

private:
    Timer() {
        global_start = std::chrono::high_resolution_clock::now();
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> global_start;
    std::unordered_map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> event_start_times;
    std::vector<std::pair<double, std::string>> events;
    mutable std::mutex mtx;
};

int main() {
    Timer& timer = Timer::getInstance();

    // Simulate events
    timer.startEvent("Task 1");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    timer.endEvent("Task 1");

    timer.startEvent("Task 2");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    timer.endEvent("Task 2");

    timer.startEvent("Task 3");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    timer.endEvent("Task 3");

    // Print the report
    timer.report();

    return 0;
}
