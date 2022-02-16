mkdir -p /source/build && \
cd /source/build && \
cmake ../ -DCMAKE_BUILD_TYPE=Debug && \
make -j4