cmake_minimum_required(VERSION 3.1)

project(tomboynotes)

############ ECM setup ###########
find_package(ECM 5.13.0 CONFIG REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)
include(ECMQtDeclareLoggingCategory)

########### Dependencies ###########

set(KF5_VERSION "5.13.0")
set(AKONADI_VERSION "5.0")
set(QT_REQUIRED_VERSION "5.3.0")

find_package(Qt5 ${QT_REQUIRED_VERSION} REQUIRED Core Network Gui)
find_package(Qt5WebEngineWidgets REQUIRED)

find_package(KF5Akonadi ${AKONADI_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiNotes ${AKONADI_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Config ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5KIO ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Mime REQUIRED)
find_package(KF5WindowSystem REQUIRED)
find_package(o2 REQUIRED)

find_program(XSLTPROC_EXECUTABLE xsltproc DOC "Path to xslt executable")

if(XSLTPROC_EXECUTABLE)
    # We depend on kdepimlibs, make sure it's found
    if(NOT DEFINED KF5Akonadi_DATA_DIR)
        find_package(KF5Akonadi REQUIRED)
    endif()

    # Macro to generate a D-Bus interface description from a KConfigXT file
    macro(kcfg_generate_dbus_interface _kcfg _name)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_name}.xml
            COMMAND ${XSLTPROC_EXECUTABLE} --stringparam interfaceName ${_name}
                    ${KF5Akonadi_DATA_DIR}/kcfg2dbus.xsl
                    ${_kcfg}
                    > ${CMAKE_CURRENT_BINARY_DIR}/${_name}.xml
            DEPENDS ${KF5Akonadi_DATA_DIR}/kcfg2dbus.xsl
            ${_kcfg}
        )
    endmacro()
endif()

remove_definitions(-DQT_NO_URL_CAST_FROM_STRING
                   -DQT_NO_CAST_FROM_BYTEARRAY
                   -DQT_USE_QSTRINGBUILDER
                   -DQT_USE_FAST_OPERATOR_PLUS
)



########### next target ###############

set(tomboynotesresource_SRCS
    configdialog.cpp
    tomboynotesresource.cpp
    tomboycollectionsdownloadjob.cpp
    tomboyitemdownloadjob.cpp
    tomboyitemuploadjob.cpp
    tomboyitemsdownloadjob.cpp
    tomboyjobbase.cpp
    tomboyserverauthenticatejob.cpp
    o1tomboy.cpp
    o2/o0settingsstore.cpp
    o2/o0baseauth.cpp
    o2/o0abstractstore.h
    o2/o0globals.h
    o2/o1.cpp
    o2/o1requestor.cpp
    o2/o1timedreply.cpp
    o2/o2.cpp
    o2/o2reply.cpp
    o2/o2replyserver.cpp
    o2/o2requestor.cpp
    o2/o2simplecrypt.cpp
)

ecm_qt_declare_logging_category(tomboynotesresource_SRCS
    HEADER debug.h
    IDENTIFIER log_tomboynotesresource
    CATEGORY_NAME log_tomboynotesresource
)

ki18n_wrap_ui(tomboynotesresource_SRCS configdialog.ui)

kconfig_add_kcfg_files(tomboynotesresource_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/settings.kcfgc
)

kcfg_generate_dbus_interface(
    ${CMAKE_CURRENT_SOURCE_DIR}/tomboynotesresource.kcfg
    org.kde.Akonadi.TomboyNotes.Settings
)

qt5_add_dbus_adaptor(tomboynotesresource_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.Akonadi.TomboyNotes.Settings.xml
    ${CMAKE_CURRENT_BINARY_DIR}/settings.h
    Settings
)


add_executable(akonadi_tomboynotes_resource ${tomboynotesresource_SRCS})
target_link_libraries(akonadi_tomboynotes_resource
    Qt5::DBus
    Qt5::Gui
    Qt5::Network
    Qt5::WebEngineWidgets
    KF5::AkonadiAgentBase
    KF5::AkonadiNotes
    KF5::ConfigCore
    KF5::ConfigWidgets
    KF5::I18n
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::KIONTLM
    KF5::KIOWidgets
    KF5::Mime
    KF5::WindowSystem
)

install(TARGETS akonadi_tomboynotes_resource ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES tomboynotesresource.desktop
        DESTINATION "${KDE_INSTALL_DATAROOTDIR}/share/akonadi/agents"
)

########### summary ############

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
