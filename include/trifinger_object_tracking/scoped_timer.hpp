#pragma once

#include <chrono>
#include <iostream>
#include <string>

class ScopedTimer
{
public:
    ScopedTimer(const std::string &name)
        : name_(name), start_time_(std::chrono::steady_clock::now())
    {
    }

    ~ScopedTimer()
    {
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time_);

        std::cout << "Duration " << name_ << ": " << duration.count() << " ms."
                  << std::endl;
    }

private:
    std::string name_;
    std::chrono::time_point<std::chrono::steady_clock> start_time_;
};