cmake_minimum_required(VERSION 3.0)

project(TestBrowser)

find_package(ECM "5.14.0" REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(FeatureSummary)

find_package(KDevPlatform 5.2.40 REQUIRED)

add_subdirectory(src)

# kdebugsettings file
if (ECM_VERSION VERSION_GREATER "5.58.0")
    install(FILES testbrowser.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
else()
    install(FILES testbrowser.categories DESTINATION ${KDE_INSTALL_CONFDIR})
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

enable_testing()
find_package(GTest)
if(GTest_FOUND)
    message(STATUS "Testing enabled: Building test host")
    add_subdirectory(tests)
else()
    message(STATUS "Testing disabled: Package gtest missing")
endif()
