# Setup vcpkg

git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh

## Fresh
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake