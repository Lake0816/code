���뷽����
1. Ŀǰ�Ǳ�����PC�����еģ�����Ҫ���豸�˱��룬��Ҫ�޸�source/CMakeLists.txt���������ݣ�
SET(TOOLCHAIN_DIR "/usr") �޸ı���·��
SET(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/gcc)   �޸ı��빤����
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/g++)  �޸ı��빤����

2. ���뷽����
(1). ����release�汾
./make.sh all
(2). ����debug�汾
./make.sh all -d
(3). ɾ������������ļ�
./make.sh clean