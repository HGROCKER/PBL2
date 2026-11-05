cmake_minimum_required(VERSION 3.16)
project(PBL2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
    main.cpp
    main_window.cpp
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
    archive_storage.h
    main_window.h
    table_helpers.h
    core/application_logic.h
    ui/logic_messages.h
)

add_executable(PBL2 ${SOURCES} ${HEADERS})

target_link_libraries(PBL2 Qt6::Core Qt6::Gui Qt6::Widgets)