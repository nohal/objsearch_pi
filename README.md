#Vector Chart Object Search  Plugin for OpenCPN
This plugin provides a search interface to the objects on the vector charts in OpenCPN. Note that it indexes the objects on the charts which were loaded at least once with the plugin enabled.

##Current CI Status
[![Build Status](https://api.travis-ci.org/nohal/objsearch_pi.svg)](http://travis-ci.org/nohal/objsearch_pi)
[![Build status](https://ci.appveyor.com/api/projects/status/em6ev0oljveg842k)](https://ci.appveyor.com/project/nohal/objsearch-pi)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/3039/badge.svg)](https://scan.coverity.com/projects/3039)

##Building
This assumes you have already seen the building instructions at http://opencpn.org/ocpn/compiling_source_linux or http://opencpn.org/ocpn/compiling_source_windows and can build OpenCPN from source.

###Linux:
```
git clone https://github.com/nohal/objsearch_pi.git
cd objsearch_pi
mkdir build
cd build
cmake ..
make
```
###Windows:
You must have opencpn.lib created while OpenCPN application is built in the build folder. If you don't want to make one yourself, there are several prebuilt versions at https://sourceforge.net/projects/opencpnplugins/files/opencpn_lib/ - you need one corresponding to version newer than 3.3.2028
```
git clone https://github.com/nohal/objsearch_pi.git
cd objsearch_pi
mkdir build
```
Copy opencpn.lib into the newly created build folder
```
cd build
cmake ..
cmake --build . --config release
```

##CSV Data Import
The format of the CSV is simple:
```Lat, Lon, Object name, Feature name,"Source" name[[,Scale],PPM Scale]```
Strings containing commas of course have to be quoted, the scale fields are optional

