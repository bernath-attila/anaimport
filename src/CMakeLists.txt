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
#add_executable(testMyClass testMyClass.cc)
#target_link_libraries(testMyClass mylib)
#target_link_libraries(testMyClass gtest gtest_main)
#add_test(testMyClass testMyClass)
