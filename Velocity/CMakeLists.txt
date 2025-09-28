add_executable(Velocity
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

target_include_directories(Velocity
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
)

# Link against Qt Core+Xml and XboxInternals (shared by default)
target_link_libraries(Velocity
  PRIVATE
    Qt6::Core
    Qt6::Xml
    XboxInternals
)

# App requires C++20
target_compile_features(Velocity PUBLIC cxx_std_20)
