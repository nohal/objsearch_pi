#Vector Chart Object Search  Plugin for OpenCPN
This plugin provides a search interface to the objects on the vector charts in OpenCPN. Note that it indexes the objects on the charts which were loaded at least once with the plugin enabled.

##Building
This assumes you have already seen the building instructions at http://opencpn.org/ocpn/compiling_source_linux or http://opencpn.org/ocpn/compiling_source_windows and can build OpenCPN from source.

###Linux:
```
cd YOUROPENCPNSOURCETREE/plugins
git clone git clone https://github.com/nohal/objsearch_pi.git
cd ../build
cmake ..
make
```
###Windows:
```
cd C:\YOUROPENCPNSOURCETREE\plugins
git clone git clone https://github.com/nohal/objsearch_pi.git
cd ..\build
cmake ..
cmake --build . --config release
```
