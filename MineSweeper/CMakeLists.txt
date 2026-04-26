cmake_minimum_required(VERSION 3.16)

project(MineSweeper VERSION 0.1 LANGUAGES CXX)

# -----------------------------------------------------------------------------
# 全局设置
# -----------------------------------------------------------------------------
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# 查找 Qt（优先 Qt6，回退到 Qt5）
# -----------------------------------------------------------------------------
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# -----------------------------------------------------------------------------
# 项目源文件列表
# -----------------------------------------------------------------------------
set(PROJECT_SOURCES
    main.cpp
    minesweeper.cpp
    minesweeper.h
    minesweeper.ui
    gameboard.cpp
    gameboard.h
    gameboard.ui
    minecell.cpp
    minecell.h
    gameconfig.cpp
    gameconfig.h
)

set(PROJECT_RESOURCES
    resources.qrc
)

# -----------------------------------------------------------------------------
# 根据 Qt 版本创建可执行文件
# -----------------------------------------------------------------------------
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    # Qt6: 使用 qt_add_executable，自动处理 moc/ui/rcc
    qt_add_executable(MineSweeper
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_RESOURCES}
        gametitle.h gametitle.cpp gametitle.ui
    )
else()
    # Qt5: 使用标准 add_executable，需要手动处理资源文件
    add_executable(MineSweeper
        ${PROJECT_SOURCES}
    )

    # 显式添加资源文件（生成 .qrc 对应的 cpp 文件）
    qt5_add_resources(RESOURCES_RCC ${PROJECT_RESOURCES})
    target_sources(MineSweeper PRIVATE ${RESOURCES_RCC})
endif()

# -----------------------------------------------------------------------------
# 链接 Qt 库
# -----------------------------------------------------------------------------
target_link_libraries(MineSweeper PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# -----------------------------------------------------------------------------
# 传递版本宏给源码
# -----------------------------------------------------------------------------
target_compile_definitions(MineSweeper PRIVATE
    APP_VERSION="${PROJECT_VERSION}"
)

# -----------------------------------------------------------------------------
# 设置目标属性（Windows、macOS、Android 等平台特性）
# -----------------------------------------------------------------------------
# macOS 应用程序 bundle 标识符（Qt < 6.1 需要手动设置）
if(${QT_VERSION_MAJOR} EQUAL 5 OR (${QT_VERSION_MAJOR} EQUAL 6 AND ${QT_VERSION} VERSION_LESS 6.1.0))
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MineSweeper)
endif()

set_target_properties(MineSweeper PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE   # 使 Windows 下运行时不显示控制台窗口
)

# Android 平台特殊处理（仅 Qt5）
if(${QT_VERSION_MAJOR} EQUAL 5 AND ANDROID)
    # 设置为共享库，以便 Android 打包
    set_target_properties(MineSweeper PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libs/${ANDROID_ABI}"
    )
    # 可选：设置 Android 包源目录
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
endif()

# -----------------------------------------------------------------------------
# 安装规则
# -----------------------------------------------------------------------------
include(GNUInstallDirs)
install(TARGETS MineSweeper
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# -----------------------------------------------------------------------------
# Qt6 最终化（必须调用）
# -----------------------------------------------------------------------------
if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(MineSweeper)
endif()
