BASE_DIR=/home/edip/Documents/ANDROID_SURVEYING_LINUX/OSGEO_QGIS/ANDROID/3.17
android {
  QGIS_INSTALL_PATH = $${BASE_DIR}/$$ANDROID_TARGET_ARCH
  #QGSQUICK_INSTALL_PATH = /home/edip/Documents/ANDROID_SURVEYING_LINUX/OSGEO_QGIS/ANDROID/3.17/qgsquick
  QGIS_QUICK_DATA_PATH = Surveying_Calculator

  GEODIFF_INCLUDE_DIR = $${QGIS_INSTALL_PATH}/include
  GEODIFF_LIB_DIR = $${QGIS_INSTALL_PATH}/lib
}
unix:!macx:!android {
  QGIS_SRC_DIR = /home/edip/workspace/QGIS_LABS/qgis_3.16
  QGIS_BUILD_DIR = /home/edip/workspace/QGIS_LABS/qgis_3.16/build-QGIS-Desktop
  QGIS_QUICK_DATA_PATH = /home/edip/Public/DATA/android_qgis
}
