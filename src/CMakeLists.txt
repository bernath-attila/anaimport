add_library(mylib mylib.cc)
add_executable(main main.cc)
target_link_libraries(main mylib)

add_executable(statsOnPairings statsOnPairings.cc)
target_link_libraries(statsOnPairings mylib)


add_executable(testMyClass testMyClass.cc)
target_link_libraries(testMyClass mylib)
target_link_libraries(testMyClass gtest gtest_main)
add_test(testMyClass testMyClass)

add_library(pair_imp pair_imp.cc)
add_executable(testPairingImport testPairingImport.cc)
target_link_libraries(testPairingImport pair_imp)
target_link_libraries(testPairingImport gtest gtest_main)
add_test(testPairingImport testPairingImport)


add_library(myPairConv myPairConv.cc)
add_executable(testMyPairConv testMyPairConv.cc)
target_link_libraries(testMyPairConv myPairConv)
target_link_libraries(testMyPairConv gtest gtest_main)
add_test(testMyPairConv testMyPairConv)

add_executable(convertPairings convertPairings.cc)
target_link_libraries(convertPairings myPairConv)

add_executable(checkConsecutiveEvents checkConsecutiveEvents.cc)
target_link_libraries(checkConsecutiveEvents myPairConv)


add_executable(createPairingsForEachDay createPairingsForEachDay.cc)
target_link_libraries(createPairingsForEachDay myPairConv)

add_executable(getMaxPairLength getMaxPairLength.cc)
target_link_libraries(getMaxPairLength myPairConv)

add_executable(changePairingContract changePairingContract.cc)
target_link_libraries(changePairingContract myPairConv)
