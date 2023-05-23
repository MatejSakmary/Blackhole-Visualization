cmake_minimum_required(VERSION 3.21)

project(blackhole-viz)


add_executable( ${PROJECT_NAME} 
    "source/main.cpp"
    "source/application.cpp"
    "source/camera.cpp"
    "source/renderer/renderer.cpp"
)

find_package(daxa CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui::imgui
    daxa::daxa
    glm::glm
    glfw
)

# Debug mode defines
target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:LOG_DEBUG>")
target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:__DEBUG__>")
