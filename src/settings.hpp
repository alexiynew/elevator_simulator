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
	static constexpr int default_floors_count = 5;
	static constexpr float default_floor_height = 2.5f; // meters
	static constexpr float default_speed = 1.0f; // meters per second
	static constexpr int default_open_time = 5; // seconds

    int m_floors_count = default_floors_count;
    float m_floor_height = default_floor_height;
    float m_speed = default_speed; 
    int m_open_time = default_open_time; 
};

#endif