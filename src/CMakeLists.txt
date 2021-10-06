set(testbrowser_SRCS
    testbrowser.cpp
    testview.cpp
    GoogleTestData.cpp
    GoogleTestGroup.cpp
    GoogleTestRunner.cpp
    IconManager.cpp
    InfoTextDelegate.cpp
    TestDataJob.cpp
    TestFactory.cpp
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
