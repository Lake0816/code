编译方法：
1. 目前是编译在PC端运行的，如需要在设备端编译，需要修改source/CMakeLists.txt中如下内容：
SET(TOOLCHAIN_DIR "/usr") 修改编译路径
SET(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/gcc)   修改编译工具链
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/g++)  修改编译工具链

2. 编译方法：
(1). 编译release版本
./make.sh all
(2). 编译debug版本
./make.sh all -d
(3). 删除编译产生的文件
./make.sh clean