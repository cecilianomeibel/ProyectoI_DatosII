cmake_minimum_required(VERSION 3.23)
project(ProyectoI_DatosII)


find_package(OpenCV REQUIRED)
set(CMAKE_CXX_STANDARD 14)

add_executable(Server server.cpp)

add_executable(Client client.cpp)


include_directories( ${OpenCV_INCLUDE_DIRS})

target_link_libraries( Server ${OpenCV_LIBS})
target_link_libraries(Server ${Boost_LIBRARIES})

target_link_libraries( Client ${OpenCV_LIBS})
target_link_libraries(Client ${Boost_LIBRARIES})
