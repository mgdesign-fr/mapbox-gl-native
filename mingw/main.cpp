#include <mbgl/mbgl_c.h>

#include <signal.h>
#include <getopt.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>

static mbgl_GLFWView_t* view = NULL;

struct DefaultStyle {
    const char* path;
    const char* name;
};

static const DefaultStyle defaultStyles[] = {
    { "asset://styles/styles/streets-v8.json", "Mapbox Streets" },
    { "asset://styles/styles/emerald-v8.json", "Emerald" },
    { "asset://styles/styles/light-v8.json", "Light" },
    { "asset://styles/styles/dark-v8.json", "Dark" },
    { "asset://styles/styles/satellite-v8.json", "Satellite" }
};

static int numDefaultStyles = (sizeof(defaultStyles) / sizeof(defaultStyles[0]));

void quit_handler(int) {
    if (view) {
        printf("[INFO] waiting for quit...");
        mbgl_GLFWView_setShouldClose(view);
    } else {
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    bool fullscreen = false;
    bool benchmark = false;
    std::string style;
    double latitude = 0, longitude = 0;
    double bearing = 0, zoom = 1;
    bool skipConfig = false;

    const struct option long_options[] = {
        {"fullscreen", no_argument, 0, 'f'},
        {"benchmark", no_argument, 0, 'b'},
        {"style", required_argument, 0, 's'},
        {"lon", required_argument, 0, 'x'},
        {"lat", required_argument, 0, 'y'},
        {"zoom", required_argument, 0, 'z'},
        {"bearing", required_argument, 0, 'r'},
        {0, 0, 0, 0}
    };

    while (true) {
        int option_index = 0;
        int opt = getopt_long(argc, argv, "fbs:", long_options, &option_index);
        if (opt == -1) break;
        switch (opt)
        {
        case 0:
            if (long_options[option_index].flag != 0)
                break;
        case 'f':
            fullscreen = true;
            break;
        case 'b':
            benchmark = true;
            break;
        case 's':
            style = std::string("asset://") + std::string(optarg);
            break;
        case 'x':
            longitude = atof(optarg);
            skipConfig = true;
            break;
        case 'y':
            latitude = atof(optarg);
            skipConfig = true;
            break;
        case 'z':
            zoom = atof(optarg);
            skipConfig = true;
            break;
        case 'r':
            bearing = atof(optarg);
            skipConfig = true;
            break;
        default:
            break;
        }

    }

    if (benchmark) {
        printf("[INFO] BENCHMARK MODE: Some optimizations are disabled.");
    }

    mbgl_GLFWView_init(fullscreen, benchmark, &view);

    mbgl_SQLiteCache_t* cache = NULL;
    mbgl_SQLiteCache_init("cache.sqlite", &cache);

    mbgl_DefaultFileSource_t* fileSource = NULL;
    mbgl_DefaultFileSource_init(cache, &fileSource);

    // Set access token if present
    const char *token = getenv("MAPBOX_ACCESS_TOKEN");
    if (token == nullptr) {
        printf("[WARNING] no access token set. mapbox.com tiles won't work.");
    } else {
        mbgl_DefaultFileSource_setAccessToken(fileSource, token);
    }

    mbgl_Map_t* map = NULL;
    mbgl_Map_init((mbgl_View_t*)view, fileSource, &map);

    mbgl_Map_setLatLngZoom(map, latitude, longitude, zoom);
    mbgl_Map_setBearing(map, bearing);
    printf("[INFO] Location: %f/%f (z%.2f, %.2f deg)", latitude, longitude, zoom, bearing);

    /*
    // Load settings
    mbgl::Settings_JSON settings;

    if (skipConfig) {
        mbgl_Map_setLatLngZoom(map, latitude, longitude, zoom);
        mbgl_Map_setBearing(map, bearing);
        printf("[INFO] Location: %f/%f (z%.2f, %.2f deg)", latitude, longitude, zoom, bearing);
    } else {
        mbgl_Map_setLatLngZoom(map, settings.latitude, settings.longitude, settings.zoom);
        mbgl_Map_setBearing(map, settings.bearing);
        mbgl_Map_setDebug(map, settings.debug);
    }
    */
    mbgl_GLFWView_setChangeStyleCallback(view, [](mbgl_GLFWView_t* view, void *userdata) {
        mbgl_Map_t* map = (mbgl_Map_t*)userdata;
        static uint8_t currentStyleIndex;

        if (++currentStyleIndex == numDefaultStyles) {
            currentStyleIndex = 0;
        }

        const auto& newStyle = defaultStyles[currentStyleIndex];
        mbgl_Map_setStyleURL(map, newStyle.path);
        mbgl_GLFWView_setWindowTitle(view, newStyle.name);

        printf("[INFO] Changed style to: %s", newStyle.path);
    }, map);

    // Load style
    if (style.empty()) {
        const auto& newStyle = defaultStyles[0];
        style = newStyle.path;
        mbgl_GLFWView_setWindowTitle(view, newStyle.name);
    }

    mbgl_Map_setStyleURL(map, style.c_str());

    mbgl_GLFWView_run(view);

    /*
    // Save settings
    mbgl::LatLng latLng = map.getLatLng();
    settings.latitude = latLng.latitude;
    settings.longitude = latLng.longitude;
    settings.zoom = map.getZoom();
    settings.bearing = map.getBearing();
    settings.debug = map.getDebug();
    if (!skipConfig) {
        settings.save();
    }
    */
    printf("[INFO] TODO !! Exit location: --lat=\"%f\" --lon=\"%f\" --zoom=\"%f\" --bearing \"%f\"",
                    latitude, longitude, zoom, bearing);

    return 0;
}
