set msString=%1
if [%msString%] == [] set msString="Visual Studio 15 2017"
echo "msString %msString%"
if not exist "build" mkdir build
pushd build
cmake -G %msString% ../src
cmake --build . --target Sample
cmake --build . --target dirlist
cmake --build . --target proclist
cmake --build . --target rot13
cmake --build . --target httpclient
cmake --build . --target wininetclient
popd
