cmake_minimum_required(VERSION 3.20)
project(SteamBonk LANGUAGES CXX)

add_executable(SteamBonk WIN32 SteamBonk.cpp)
set_target_properties(SteamBonk PROPERTIES CXX_STANDARD 17)
target_link_libraries(SteamBonk PRIVATE "winmm.lib")