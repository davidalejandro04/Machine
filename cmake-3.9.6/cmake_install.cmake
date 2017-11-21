# Install script for directory: /home/jotunheim/Mashine/cmake-3.9.6

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/doc/cmake-3.9" TYPE FILE FILES "/home/jotunheim/Mashine/cmake-3.9.6/Copyright.txt")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake-3.9" TYPE DIRECTORY PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ DIR_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/jotunheim/Mashine/cmake-3.9.6/Help"
    "/home/jotunheim/Mashine/cmake-3.9.6/Modules"
    "/home/jotunheim/Mashine/cmake-3.9.6/Templates"
    REGEX "/[^/]*\\.sh[^/]*$" PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE REGEX "Help/dev($|/)" EXCLUDE)
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/jotunheim/Mashine/cmake-3.9.6/Source/kwsys/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Utilities/KWIML/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Utilities/cmlibrhash/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Utilities/cmzlib/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Utilities/cmcurl/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Utilities/cmcompress/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Utilities/cmexpat/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Utilities/cmbzip2/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Utilities/cmliblzma/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Utilities/cmlibarchive/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Utilities/cmjsoncpp/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Utilities/cmlibuv/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Source/CursesDialog/form/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Source/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Utilities/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Tests/cmake_install.cmake")
  include("/home/jotunheim/Mashine/cmake-3.9.6/Auxiliary/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/jotunheim/Mashine/cmake-3.9.6/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
