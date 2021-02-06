android {
    message("Building ANDROID")
    message("ANDROID Platform: $${ANDROID_TARGET_ARCH}")

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

    DEFINES += MOBILE_OS

    isEmpty(QGIS_INSTALL_PATH) {
      error("Missing QGIS_INSTALL_PATH")
    }

    # using installed QGIS
    QGIS_PREFIX_PATH = $${QGIS_INSTALL_PATH}
    QGIS_LIB_DIR = $${QGIS_INSTALL_PATH}/lib
    QGIS_INCLUDE_DIR += \
           $${QGIS_INSTALL_PATH}/include \
           $${QGIS_INSTALL_PATH}/include/qgis

    QGIS_QML_DIR = $${QGIS_INSTALL_PATH}/qml

    exists($${QGIS_LIB_DIR}/libqgis_core_$${ANDROID_TARGET_ARCH}.so) {
      message("Building from QGIS: $${QGIS_LIB_DIR}/libqgis_core_$${ANDROID_TARGET_ARCH}.so")
    } else {
      error("Missing QGIS Core library in $${QGIS_LIB_DIR}/libqgis_core_$${ANDROID_TARGET_ARCH}.so")
    }

    INCLUDEPATH += $${QGIS_INCLUDE_DIR}
    LIBS += -L$${QGIS_LIB_DIR}
    LIBS += -lqgis_core_$${ANDROID_TARGET_ARCH} -lqgis_quick_$${ANDROID_TARGET_ARCH}


    QT += printsupport
    QT += androidextras

    QMAKE_CXXFLAGS += -std=c++11

    # files from this folder will be added to the package
    # (and will override any default files from Qt - template is in $QTDIR/src/android)
    QT_LIBS_DIR = $$dirname(QMAKE_QMAKE)/../lib

    DISTFILES += android/AndroidManifest.xml \
        android/build.gradle \
        android/res/xml/file_paths.xml

# packaging
    ANDROID_EXTRA_LIBS += \
        $${QGIS_LIB_DIR}/libcrypto_1_1.so \
        $${QGIS_LIB_DIR}/libpng16.so \
        $${QGIS_LIB_DIR}/libexpat.so \
        $${QGIS_LIB_DIR}/libgeodiff.so \
        $${QGIS_LIB_DIR}/libgeos.so \
        $${QGIS_LIB_DIR}/libgeos_c.so \
        $${QGIS_LIB_DIR}/libsqlite3.so \
        $${QGIS_LIB_DIR}/libcharset.so \
        $${QGIS_LIB_DIR}/libiconv.so \
        $${QGIS_LIB_DIR}/libfreexl.so \
        $${QGIS_LIB_DIR}/libgdal.so \
        $${QGIS_LIB_DIR}/libproj.so \
        $${QGIS_LIB_DIR}/libspatialindex.so \
        $${QGIS_LIB_DIR}/libpq.so \
        $${QGIS_LIB_DIR}/libspatialite.so \
        $${QGIS_LIB_DIR}/libprotobuf-lite.so \
        $${QGIS_LIB_DIR}/libqca-qt5_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libqgis_core_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libqgis_quick_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libqgis_native_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libqt5keychain_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libzip.so \
        $${QGIS_LIB_DIR}/libspatialiteprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libdelimitedtextprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libgpxprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libmssqlprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libowsprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libpostgresprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libspatialiteprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libssl_1_1.so \
        $${QGIS_LIB_DIR}/libwcsprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libwfsprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libwmsprovider_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5OpenGL_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5PrintSupport_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5Sensors_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5Network_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5Sql_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5Svg_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5AndroidExtras_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5SerialPort_$${ANDROID_TARGET_ARCH}.so



    deployment.files += /android/assets/qml/OfflineStorage/Databases/e04d88072f90f86a07481418b8ff4b6b.sqlite
    deployment.path = /android/assets/qml/OfflineStorage/Databases
    INSTALLS += deployment

    ANDROID_EXTRA_PLUGINS += $${QGIS_QML_DIR}
}
