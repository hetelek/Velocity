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
  stfsvolumedescriptordialog.cpp
  themecreationwizard.cpp
  textdialog.cpp
  titleidfinder.cpp
  titleidfinderdialog.cpp
  transferflagsdialog.cpp
  xdbfdialog.cpp
  xmldialog.cpp
  zipviewer.cpp
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
  xmldialog.ui
  textdialog.ui
  zipviewer.ui
)

# Platform-specific executable properties
if(APPLE)
    # macOS bundle configuration
    set(MACOSX_BUNDLE_GUI_IDENTIFIER com.velocitynext.app)
    set(MACOSX_BUNDLE_BUNDLE_NAME VelocityNext)
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_ICON_FILE velocity.icns)
    
    add_executable(VelocityNext MACOSX_BUNDLE ${SOURCES} ${UI_FORMS} Resources.qrc)
    
else()
    # Windows/Linux executable
    add_executable(VelocityNext ${SOURCES} ${UI_FORMS} Resources.qrc)
    
    if(WIN32)
        set_target_properties(VelocityNext PROPERTIES WIN32_EXECUTABLE TRUE)
        target_sources(VelocityNext PRIVATE velocity.rc)
    endif()
endif()

# Common include directories for all platforms
target_include_directories(VelocityNext
  PRIVATE
    # Current directory for header files co-located with source
    ${CMAKE_CURRENT_LIST_DIR}
    # Qt private headers for advanced features
    ${Qt6Core_PRIVATE_INCLUDE_DIRS}
)

# Common linking for all platforms - ALWAYS use our BotanAmalgamation
target_link_libraries(VelocityNext
  PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Xml
    Qt6::Widgets  
    Qt6::Network
    XboxInternals::XboxInternals  # Use namespaced target (modern CMake)
    BotanAmalgamation  # Our custom amalgamation on ALL platforms
    velocity_compiler_flags  # Inherit common compile options
)

# Define VERSION and platform build name
target_compile_definitions(VelocityNext 
  PRIVATE 
    VERSION="\\\"${PROJECT_VERSION}\\\""
    PLATFORM_BUILD_NAME="${PLATFORM_BUILD_NAME}"
)

# App requires C++20
target_compile_features(VelocityNext PUBLIC cxx_std_20)

# Organize in IDE (Visual Studio solution explorer)
set_target_properties(VelocityNext PROPERTIES FOLDER "Applications")