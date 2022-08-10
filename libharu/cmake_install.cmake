# Install script for directory: C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/libharu")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_types.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_consts.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_version.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_annotation.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_catalog.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_conf.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_destination.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_doc.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_encoder.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_encrypt.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_encryptdict.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_error.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_ext_gstate.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_font.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_fontdef.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_gstate.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_image.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_info.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_list.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_mmgr.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_objects.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_outline.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_pages.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_page_label.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_streams.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_u3d.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_utils.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_pdfa.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_3dmeasure.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_exdata.h"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/include/hpdf_config.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/README"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/CHANGES"
    "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/INSTALL"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE DIRECTORY FILES "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/if")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/src/cmake_install.cmake")
  include("C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/demo/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/rodoc/HOME/developpement/visual/StochasticDithering/libharu/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
