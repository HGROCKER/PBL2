cmake_minimum_required(VERSION 3.16)
project(PLB2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
    main.cpp
    csdl.cpp
)

# ✅ FIX: Khai báo đầy đủ toàn bộ các file .h để bộ chuyển đổi MOC của Qt không bị lỗi biên dịch link-time
set(HEADERS
    database.h
    vector.h
    csdl.h
    search_features.h
    match_widget.h
    player_widget.h
    team_widget.h
    tournament_widget.h
)

add_executable(PLB2 ${SOURCES} ${HEADERS})

target_link_libraries(PLB2 Qt6::Core Qt6::Gui Qt6::Widgets)
