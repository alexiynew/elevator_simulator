#ifndef SETTINGS_HPP
#define SETTINGS_HPP

class settings {
public:
    settings(int argc, char* argv[]);

    int floors_count() const { return m_floors_count; }
    float floor_height() const { return m_floor_height; }
    float speed() const { return m_speed; }
    int open_time() const { return m_open_time; }

private:
    int m_floors_count = 5;
    float m_floor_height = 2.5f; // meters
    float m_speed = 1.0f; // meters per second
    int m_open_time = 5; // seconds
};

#endif