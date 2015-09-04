#include "default_styles.hpp"

namespace mbgl {
namespace util {

const std::vector<std::pair<std::string, std::string>> defaultStyles = {
    { "asset://styles/styles/streets-v8.json", "Mapbox Streets" },
    { "asset://styles/styles/emerald-v8.json", "Emerald" },
    { "asset://styles/styles/light-v8.json", "Light" },
    { "asset://styles/styles/dark-v8.json", "Dark" },
    { "asset://styles/styles/satellite-v8.json", "Satellite" }
};

} // end namespace util
} // end namespace mbgl
