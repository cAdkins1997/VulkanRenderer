#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include "vulkan/Application.h"

int main() {
    rendering::Application application{};

    try {
        application.run();
    }
    catch (const std::exception &e) {
            std::cerr << e.what() << '\n';
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}