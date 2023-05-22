#include <stdexcept>
#include <iostream>

#include "application.hpp"

int main()
{
    Application application = {};

    application.main_loop();

    return EXIT_SUCCESS;
}
    