project(plasma-applet-presence)

cmake_minimum_required(VERSION 2.8)

find_package(KDE4 REQUIRED)

find_program(TELEPATHY_KDE_CONTACTLIST NAME telepathy-kde-contactlist PATHS ${KDE4_BIN_INSTALL_DIR} NO_DEFAULT_PATH)

if(NOT TELEPATHY_KDE_CONTACTLIST)
    message(FATAL_ERROR "Telepathy-kde-contactlist is required for the correct functioning of this plasmoid")
endif(NOT TELEPATHY_KDE_CONTACTLIST)

# TODO find out how to find kcm_telepathy_accounts
# find_library(KCM_TELEPATHY_ACCUNTS NAME kcm_telepathy_accounts PATHS ${KDE4_LIB_INSTALL_DIR} NO_DEFAULT_PATH)

# message("accounts DIR -> " ${KCM_TELEPATHY_ACCUNTS})

# if(NOT KCM_TELEPATHY_ACCUNTS)
#     message(FATAL_ERROR "kcm_telepathy_accounts is required for the correct functioning of this plasmoid")
# endif(NOT KCM_TELEPATHY_ACCUNTS)

include(KDE4Defaults)

add_definitions(${KDE4_DEFINITIONS})

include_directories(
    ${KDE4_INCLUDES}
)

install(DIRECTORY package/ DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/org.kde.telepathy-presence PATTERN .git EXCLUDE PATTERN CMakeLists.txt EXCLUDE)

install(FILES metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR} RENAME metadata.desktop)