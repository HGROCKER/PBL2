cmake_minimum_required(VERSION 3.16)
project(PLB2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Tìm thư viện Qt6 (Vì máy bạn đang chạy bộ Kit Qt 6.11.2 MinGW)
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# Nhận diện thư mục hiện tại để xử lý tệp tin tiêu đề .h
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# KHAI BÁO ĐỦ CÁC FILE .CPP THỰC TẾ
set(SOURCES
    main.cpp
    csdl.cpp
)

# ✅ SỬA: Khai báo thêm file search_features.h vào hệ thống
set(HEADERS
    database.h
    vector.h
    csdl.h
    search_features.h
)

add_executable(PLB2 ${SOURCES} ${HEADERS})

# Liên kết các module đồ họa giao diện của Qt6
target_link_libraries(PLB2 Qt6::Core Qt6::Gui Qt6::Widgets)
