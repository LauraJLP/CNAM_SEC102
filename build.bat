pushd build
cmake -G "Visual Studio 15 2017" ../src
cmake --build . --target Sample
cmake --build . --target dirlist
cmake --build . --target proclist
cmake --build . --target rot13
popd
