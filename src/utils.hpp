#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <string>

namespace utils {

template <typename T>
T value_or(const std::string& string, const T default_value) 
{
    std::stringstream temp(string);
    T value = default_value;
    if (temp >> value) {
    	return value;
    }

    return default_value;
}

template<typename T>
std::string to_string(const T value)
{
    std::stringstream temp;
    temp << value;
    return temp.str();
}

}


#endif