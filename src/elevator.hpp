#ifndef ELEVATOR_HPP
#define ELEVATOR_HPP

#include "settings.hpp"
#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class elevator {
public:
    using message_handler = std::function<void(const std::string&)>;
    elevator(const settings& setup);
    ~elevator();

    void run();
    void stop();

    void move_to(int floor);
    void call_to(int floor);

    void set_message_handler(message_handler handler);

private:
    enum class state {
        waiting,
        moving,
        opening_doors,
        waiting_doors,
        closing_doors,
    };

    enum class direction {
        up,
        down,
        none
    };

    const settings m_setup;
    const std::chrono::milliseconds m_time_for_one_floor;
    const std::chrono::milliseconds m_time_for_open_doors;

    message_handler m_message_handler = nullptr;

    state m_state = state::waiting;
    direction m_direction = direction::none;
    bool m_working = false;
    int m_current_floor = 1;
    int m_target_floor = 1;

    std::mutex m_mutex;
    std::vector<bool> m_requested_floors;
    std::thread m_thread;

    std::chrono::high_resolution_clock::time_point m_moving_start_time;
    std::chrono::high_resolution_clock::time_point m_doors_open_time;

    void process_requests();
    void update();

    void send_message(const std::string& message);

    void add_request(int floor);
    void complete_request(int floor);
    bool is_available_floor(int floor) const;

    int get_next_floor();
    int next_upper_floor();
    int next_lower_floor();
};

#endif