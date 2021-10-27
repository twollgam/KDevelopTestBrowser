set(testbrowser_SRCS
    testbrowser.cpp
    TestView.cpp
    GoogleTest.cpp
    GoogleTestGroup.cpp
    GoogleTestRunner.cpp
    GoogleTestSuite.cpp
    IconManager.cpp
    InfoTextDelegate.cpp
    SelectAll.cpp
    SelectFailed.cpp
    TestDataJob.cpp
    TestFactory.cpp
    TextBrowser.cpp
    TreeView.cpp
    utilities.cpp
)

ecm_qt_declare_logging_category(testbrowser_SRCS
    HEADER debug.h
    IDENTIFIER PLUGIN_TESTBROWSER
    CATEGORY_NAME "kdevelop.plugins.testbrowser"
)

qt5_add_resources(testbrowser_SRCS testbrowser.qrc)

kdevplatform_add_plugin(testbrowser
    JSON testbrowser.json
    SOURCES ${testbrowser_SRCS}
)

target_link_libraries(testbrowser
    #KF5::ItemModels 
    KDev::Interfaces 
    KDev::Util 
    KDev::Language
    KDev::Project
)
