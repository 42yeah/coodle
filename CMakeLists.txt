cmake_minimum_required(VERSION 3.0)

project(coodle)

find_package(oatpp REQUIRED)
find_package(oatpp-websocket REQUIRED)

add_executable(coodle main.cpp websocket/wslistener.cpp controller/doodlecontroller.cpp doodle.cpp websocket/doodlesync.cpp)

target_link_libraries(coodle PUBLIC
    oatpp::oatpp
    oatpp::oatpp-websocket)

install(TARGETS coodle RUNTIME DESTINATION bin)
