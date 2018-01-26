#include "elevator.hpp"
#include "utils.hpp"
#include <cassert>

using namespace std::chrono_literals;

namespace {
constexpr std::chrono::milliseconds sleep_time = 20ms;
}

elevator::elevator(const settings& setup)
    : m_setup(setup)
    , m_time_for_one_floor(static_cast<int>((setup.floor_height() / setup.speed()) * std::milli::den))
    , m_time_for_open_doors(setup.doors_open_time() * std::milli::den)
    , m_requested_floors(setup.floors_count(), false)
{
}

elevator::~elevator()
{
    stop();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void elevator::run()
{
    m_thread = std::thread(&elevator::process_requests, this);

    std::lock_guard<std::mutex> lock(m_mutex);

    m_working = true;
}

void elevator::stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::fill(m_requested_floors.begin(), m_requested_floors.end(), false);
    m_working = false;
}

void elevator::move_to(int floor)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_working) {
        return;
    }
    add_request(floor);
}

void elevator::call_to(int floor)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_working || m_state != state::waiting) {
        return;
    }

    add_request(floor);
}

void elevator::set_message_handler(message_handler handler)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_message_handler = std::move(handler);
}

void elevator::process_requests()
{
    while (m_working) {
        update();
        std::this_thread::sleep_for(sleep_time);
    }
}

void elevator::update()
{
    auto select_next_floor = [this]() {
        if (int next_floor = get_next_floor()) {
            m_target_floor = next_floor;
            assert(is_available_floor(m_target_floor));
            if (m_target_floor != m_current_floor) {
                m_direction = m_target_floor > m_current_floor ? direction::up : direction::down;
                m_moving_start_time = std::chrono::high_resolution_clock::now();
                m_state = state::moving;
            } else {
                m_state = state::opening_doors;
            }
            return true;
        }
        return false;
    };

    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_working) {
        return;
    }

    switch (m_state) {
    case state::waiting: {
        select_next_floor();
        break;
    }
    case state::moving: {
        if (int next_floor = get_next_floor()) {
            if (next_floor != m_current_floor) {
                m_target_floor = next_floor;
            }
        }

        if (m_current_floor == m_target_floor) {
            m_state = state::opening_doors;
            break;
        }

        const auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - m_moving_start_time;

        if (duration >= m_time_for_one_floor) {
            m_moving_start_time = std::chrono::high_resolution_clock::now();
            m_current_floor = m_current_floor > m_target_floor ? m_current_floor - 1 : m_current_floor + 1;
            assert(is_available_floor(m_current_floor));
            send_message(std::string("floor ") + utils::to_string(m_current_floor));
        }
        break;
    }
    case state::opening_doors: {
        m_doors_open_time = std::chrono::high_resolution_clock::now();
        assert(m_current_floor == m_target_floor);
        send_message("open doors");
        m_state = state::waiting_doors;
        break;
    }
    case state::waiting_doors: {
        const auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - m_doors_open_time;

        if (duration >= m_time_for_open_doors) {
            assert(m_current_floor == m_target_floor);
            m_state = state::closing_doors;
        }
        break;
    }
    case state::closing_doors: {
        assert(m_current_floor == m_target_floor);

        send_message("close doors");
        complete_request(m_current_floor);

        if (select_next_floor()) {
            break;
        }

        m_direction = direction::none;
        if (select_next_floor()) {
            break;
        }

        m_state = state::waiting;
        break;
    }
    }
}

void elevator::send_message(const std::string& message)
{
    if (m_message_handler) {
        m_message_handler(message);
    }
}

void elevator::add_request(int floor)
{
    if (is_available_floor(floor)) {
        m_requested_floors[floor - 1] = true;
    } else {
        send_message("selected floor is unavailable");
    }
}

void elevator::complete_request(int floor)
{
    assert(is_available_floor(floor));
    m_requested_floors[floor - 1] = false;
}

bool elevator::is_available_floor(int floor) const
{
    return floor > 0 && floor <= m_setup.floors_count();
}

int elevator::get_next_floor()
{
    int floor = 0;
    switch (m_direction) {
    case direction::up:
        floor = next_upper_floor();
        break;
    case direction::down:
        floor = next_lower_floor();
        break;
    case direction::none:
        floor = next_upper_floor();
        if (!floor) {
            floor = next_lower_floor();
        }
        break;
    }

    return floor;
}

int elevator::next_upper_floor()
{
    const int size = m_requested_floors.size();
    for (int i = m_current_floor - 1; i < size; ++i) {
        if (m_requested_floors[i]) {
            return i + 1;
        }
    }
    return 0;
};

int elevator::next_lower_floor()
{
    for (int i = m_current_floor - 1; i >= 0; --i) {
        if (m_requested_floors[i]) {
            return i + 1;
        }
    }
    return 0;
};