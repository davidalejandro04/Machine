if(NOT "/home/jotunheim/Mashine/cmake-3.9.6/Tests/CMakeTests" MATCHES "^/")
  set(slash /)
endif()
set(url "file://${slash}/home/jotunheim/Mashine/cmake-3.9.6/Tests/CMakeTests/FileDownloadInput.png")
set(dir "/home/jotunheim/Mashine/cmake-3.9.6/Tests/CMakeTests/downloads")

file(DOWNLOAD
  ${url}
  ${dir}/file3.png
  TIMEOUT 2
  STATUS status
  EXPECTED_HASH SHA1=5555555555555555555555555555555555555555
  )
