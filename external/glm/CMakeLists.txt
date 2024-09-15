cmake_minimum_required(VERSION 3.14)
# Fetch and include glm
include(FetchContent)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.0
)
FetchContent_MakeAvailable(glm)