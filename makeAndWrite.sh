rm -rf build
mkdir build
cd build
cmake ..
make -j4
picotool load tiny_usb_midi2.uf2 # This might require sudo.
cd ..
