set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES
  about.cpp
  achievementcreationwizard.cpp
  addressconverterdialog.cpp
  avatarassetdownloader.cpp
  certificatedialog.cpp
  clustertooldialog.cpp
  creationwizard.cpp
  deviceviewer.cpp
  dragdroptreewidget.cpp
  fatxfiledialog.cpp
  fatxpathgendialog.cpp
  flashdriveconfigdatadialog.cpp
  gameadderdialog.cpp
  gamerpicturepackdialog.cpp
  githubcommitsdialog.cpp
  gpddownloader.cpp
  gpduploader.cpp
  imagedialog.cpp
  licensingdatadialog.cpp
  main.cpp
  mainwindow.cpp
  metadata.cpp
  multiprogressdialog.cpp
  nightcharts.cpp
  packageviewer.cpp
  partitiondialog.cpp
  preferencesdialog.cpp
  profilecleanerwizard.cpp
  profilecreatorwizard.cpp
  profileeditor.cpp
  propertiesdialog.cpp
  qthelpers.cpp
  renamedialog.cpp
  securitysectordialog.cpp
  singleprogressdialog.cpp
  stfstoolsdialog.cpp
  stfsvolumedescriptordialog.cpp
  strbdialog.cpp
  svoddialog.cpp
  svodfileinfodialog.cpp
  svodtooldialog.cpp
  svodvolumedescriptordialog.cpp
  themecreationwizard.cpp
  titleidfinder.cpp
  titleidfinderdialog.cpp
  transferflagsdialog.cpp
  xdbfdialog.cpp
  ytgrdialog.cpp
)

set(UI_FORMS
  mainwindow.ui
  packageviewer.ui
  profileeditor.ui
  about.ui
  metadata.ui
  certificatedialog.ui
  licensingdatadialog.ui
  transferflagsdialog.ui
  xdbfdialog.ui
  strbdialog.ui
  imagedialog.ui
  renamedialog.ui
  creationwizard.ui
  achievementcreationwizard.ui
  propertiesdialog.ui
  addressconverterdialog.ui
  stfstoolsdialog.ui
  singleprogressdialog.ui
  profilecreatorwizard.ui
  themecreationwizard.ui
  gameadderdialog.ui
  titleidfinderdialog.ui
  gamerpicturepackdialog.ui
  preferencesdialog.ui
  githubcommitsdialog.ui
  fatxpathgendialog.ui
  profilecleanerwizard.ui
  svoddialog.ui
  svodfileinfodialog.ui
  svodtooldialog.ui
  svodvolumedescriptordialog.ui
  stfsvolumedescriptordialog.ui
  multiprogressdialog.ui
  ytgrdialog.ui
  deviceviewer.ui
  clustertooldialog.ui
  fatxfiledialog.ui
  partitiondialog.ui
  securitysectordialog.ui
  flashdriveconfigdatadialog.ui
)

# Platform-specific executable properties
if(APPLE)
    # macOS bundle configuration
    set(MACOSX_BUNDLE_GUI_IDENTIFIER com.velocity.app)
    set(MACOSX_BUNDLE_BUNDLE_NAME Velocity)
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_ICON_FILE velocity.icns)
    
    add_executable(Velocity MACOSX_BUNDLE ${SOURCES} ${UI_FORMS} Resources.qrc)
    
else()
    # Windows/Linux executable
    add_executable(Velocity ${SOURCES} ${UI_FORMS} Resources.qrc)
    
    if(WIN32)
        set_target_properties(Velocity PROPERTIES WIN32_EXECUTABLE TRUE)
        target_sources(Velocity PRIVATE velocity.rc)
    endif()
endif()

# Common include directories for all platforms
target_include_directories(Velocity
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
)

# Common linking for all platforms - ALWAYS use our BotanAmalgamation
target_link_libraries(Velocity
  PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Xml
    Qt6::Widgets  
    Qt6::Network
    XboxInternals
    BotanAmalgamation  # Our custom amalgamation on ALL platforms
)

# Define VERSION and platform build name
target_compile_definitions(Velocity 
  PRIVATE 
    VERSION="\\\"${PROJECT_VERSION}\\\""
    PLATFORM_BUILD_NAME="${PLATFORM_BUILD_NAME}"
)

# App requires C++20
target_compile_features(Velocity PUBLIC cxx_std_20)