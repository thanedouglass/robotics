git clone https://github.com/google/googletest
mkdir -p ~/.local
cd googletest
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=~/.local ..
make install
cd ../..
rm -rf googletest

