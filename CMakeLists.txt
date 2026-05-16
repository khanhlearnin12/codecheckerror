cmake_minimum_required(VERSION 3.16)
project(QtGuiExample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tự động xử lý các cơ chế sinh mã đặc biệt của Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Tìm thư viện Qt (Đổi thành Qt5 nếu bạn xài Qt5)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Khai báo file exe đầu ra và file source code
add_executable(gui main.cpp CodeCheckerWindow.cpp)

# Liên kết thư viện đồ họa vào file exe
target_link_libraries(gui PRIVATE Qt6::Widgets)
