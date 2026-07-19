cmake_minimum_required(VERSION 3.21)
project(qt_switch_batches LANGUAGES CXX)

option(QT_SWITCH_BUILD_QTDECLARATIVE "Build the QtDeclarative batch" ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/QtSwitchBatch.cmake")

qt_switch_add_batch(qtbase_tests_batch
    MANIFEST "${CMAKE_CURRENT_LIST_DIR}/qtbase-manifest.txt"
    RUNNER_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/cmake/qbatchedtestrunner.in.cpp"
    INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/src/testlib"
        "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/tests/auto/other/qvariant_common"
    LIBRARIES
        Qt6::Core
        Qt6::CorePrivate
        Qt6::Test
        Qt6::TestPrivate
)

qt_switch_add_batch(qtbase_extra_tests_batch
    MANIFEST "${CMAKE_CURRENT_LIST_DIR}/qtbase-extra-manifest.txt"
    RUNNER_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/cmake/qbatchedtestrunner.in.cpp"
    INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/src/testlib"
    LIBRARIES
        Qt6::Core
        Qt6::CorePrivate
        Qt6::Test
        Qt6::TestPrivate
)

qt_switch_add_batch(qtbase_thread_tests_batch
    MANIFEST "${CMAKE_CURRENT_LIST_DIR}/qtbase-thread-manifest.txt"
    RUNNER_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/cmake/qbatchedtestrunner.in.cpp"
    INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/src/testlib"
    LIBRARIES
        Qt6::Core
        Qt6::CorePrivate
        Qt6::Test
        Qt6::TestPrivate
)

qt_switch_add_batch(qtbase_text_tests_batch
    MANIFEST "${CMAKE_CURRENT_LIST_DIR}/qtbase-text-manifest.txt"
    RUNNER_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/cmake/qbatchedtestrunner.in.cpp"
    INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/src/testlib"
    LIBRARIES
        Qt6::Core
        Qt6::CorePrivate
        Qt6::Test
        Qt6::TestPrivate
)

qt_switch_add_batch(qtbase_misc_tests_batch
    MANIFEST "${CMAKE_CURRENT_LIST_DIR}/qtbase-misc-manifest.txt"
    RUNNER_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/cmake/qbatchedtestrunner.in.cpp"
    INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/src/testlib"
    LIBRARIES
        Qt6::Core
        Qt6::CorePrivate
        Qt6::Test
        Qt6::TestPrivate
)

qt_switch_add_batch(qtbase_broad_tests_batch
    MANIFEST "${CMAKE_CURRENT_LIST_DIR}/qtbase-broad-manifest.txt"
    RUNNER_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/cmake/qbatchedtestrunner.in.cpp"
    INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/src/testlib"
    LIBRARIES
        Qt6::Core
        Qt6::CorePrivate
        Qt6::Test
        Qt6::TestPrivate
)

qt_switch_add_batch(qtbase_value_tests_batch
    MANIFEST "${CMAKE_CURRENT_LIST_DIR}/qtbase-value-manifest.txt"
    RUNNER_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/cmake/qbatchedtestrunner.in.cpp"
    INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/src/testlib"
    LIBRARIES
        Qt6::Core
        Qt6::CorePrivate
        Qt6::Test
        Qt6::TestPrivate
)

qt_switch_add_batch(qtbase_serialization_tests_batch
    MANIFEST "${CMAKE_CURRENT_LIST_DIR}/qtbase-serialization-manifest.txt"
    RUNNER_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/cmake/qbatchedtestrunner.in.cpp"
    INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/src/testlib"
    LIBRARIES
        Qt6::Core
        Qt6::CorePrivate
        Qt6::Test
        Qt6::TestPrivate
)

if(QT_SWITCH_BUILD_QTDECLARATIVE)
    find_package(Qt6 REQUIRED COMPONENTS Qml QmlModels)
    # Static QML modules need their plugin target linked explicitly. This is
    # the same embedded deployment model used by quick-app and ultimate-test;
    # no Qt runtime tree is required on the SD card.
    find_package(Qt6modelsplugin CONFIG REQUIRED)
    find_package(Qt6workerscriptplugin CONFIG REQUIRED)
    find_package(Qt6qmlplugin CONFIG REQUIRED)
    qt_switch_add_batch(qtdeclarative_tests_batch
        MANIFEST "${CMAKE_CURRENT_LIST_DIR}/qtdeclarative-manifest.txt"
        RUNNER_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/cmake/qbatchedtestrunner.in.cpp"
        INCLUDE_DIRECTORIES
            "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/src/testlib"
            "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtdeclarative/src/qml"
            "${CMAKE_CURRENT_LIST_DIR}/../../build/qtdeclarative-switch/include/QtQml/6.8.3"
        COMPILE_DEFINITIONS
            QT_SWITCH_BATCH_QML
        LIBRARIES
            Qt6::Core
            Qt6::Qml
            Qt6::QmlPrivate
            Qt6::qmlplugin
            Qt6::modelsplugin
            Qt6::workerscriptplugin
            Qt6::Test
            Qt6::TestPrivate
    )
    qt_switch_add_batch(qtdeclarative_extra_tests_batch
        MANIFEST "${CMAKE_CURRENT_LIST_DIR}/qtdeclarative-extra-manifest.txt"
        RUNNER_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/cmake/qbatchedtestrunner.in.cpp"
        INCLUDE_DIRECTORIES
            "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/src/testlib"
            "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtdeclarative/src/qml"
            "${CMAKE_CURRENT_LIST_DIR}/../../build/qtdeclarative-switch/include/QtQml/6.8.3"
        COMPILE_DEFINITIONS
            QT_SWITCH_BATCH_QML
        LIBRARIES
            Qt6::Core
            Qt6::Qml
            Qt6::QmlPrivate
            Qt6::qmlplugin
            Qt6::modelsplugin
            Qt6::workerscriptplugin
            Qt6::Test
            Qt6::TestPrivate
    )
    qt_switch_add_batch(qtdeclarative_v4_tests_batch
        MANIFEST "${CMAKE_CURRENT_LIST_DIR}/qtdeclarative-v4-manifest.txt"
        RUNNER_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/cmake/qbatchedtestrunner.in.cpp"
        INCLUDE_DIRECTORIES
            "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/src/testlib"
            "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtdeclarative/src/qml"
            "${CMAKE_CURRENT_LIST_DIR}/../../build/qtdeclarative-switch/include/QtQml/6.8.3"
        COMPILE_DEFINITIONS
            QT_SWITCH_BATCH_QML
        LIBRARIES
            Qt6::Core
            Qt6::Qml
            Qt6::QmlPrivate
            Qt6::QmlModels
            Qt6::QmlModelsPrivate
            Qt6::qmlplugin
            Qt6::modelsplugin
            Qt6::workerscriptplugin
            Qt6::Test
            Qt6::TestPrivate
    )
    qt_switch_add_batch(qtdeclarative_meta_tests_batch
        MANIFEST "${CMAKE_CURRENT_LIST_DIR}/qtdeclarative-meta-manifest.txt"
        RUNNER_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/cmake/qbatchedtestrunner.in.cpp"
        INCLUDE_DIRECTORIES
            "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtbase/src/testlib"
            "${CMAKE_CURRENT_LIST_DIR}/../../third_party/qtdeclarative/src/qml"
            "${CMAKE_CURRENT_LIST_DIR}/../../build/qtdeclarative-switch/include/QtQml/6.8.3"
        COMPILE_DEFINITIONS
            QT_SWITCH_BATCH_QML
        LIBRARIES
            Qt6::Core
            Qt6::Qml
            Qt6::QmlPrivate
            Qt6::qmlplugin
            Qt6::modelsplugin
            Qt6::workerscriptplugin
            Qt6::Test
            Qt6::TestPrivate
    )
endif()
