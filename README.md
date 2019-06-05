# sandstream

The coding challenge solution.

## Description

Naive Mpeg2 Transport Stream demultiplexer tool with functionality to extract all available elementary streams from MPEG2TS file.
Run without arguments prints usage example.

## End-User Information

Single elementary stream per run. Supports only PES-packetized elementary streams. Doesn't support stream format changes in stream or PAT/PMT updates.

### How-to use

To get all elementary streams prefixed with name 'elephants_' use:
    
    sandstream elephants.ts elephants_


## Dev Docs 

 IO level for input separated from main demuxer implementation to handle multiple IO backend. Demuxer handles one TS packet per call so there is no limit on input buffer granularity.
 IO can provide as many bytes as available without additional alignement per packet boundary.
 Demuxer skips unneeded bytes and restore bytestream synchronization on sync_byte. During implementation extended Mpeg2TS formats taken into account as BDAV with 4 extra
 bytes prepending TS packet and/or  ATSC streams with FEC bytes in front of every packet. All these extra bytes silently skipped by demuxing on resync step.
 
 Extra data copiyng has been avoided so demultiplexing process happens on every packet. Instead of copying and merging incapsulated payload the scatter/gather algorithm is used;
 storage abstraction keeps multiple packet pointers and provide api to underlying byte sequence.

 ### Tips and trick
 "version.h" and "birthday.h" provides a generic way to update internal application version and date of build. The values are printed then on run as 
    
    sandstream.exe :: 0.0.0+g0000000 :: 2019-05-22T08:08:39Z
   
### How-to build

    mkdir build && cd build
    cmake ..
    cmake --build .

Additionaly you can run unittests being in build subdirectory:

    ctest # just run and get exit status code
    ctest -V # prints test runner output

Optionally you can pack application to platform-supported package format.
    
    cpack -G TGZ # For just archiving; Tested on Arch Linux
    cpack -G RPM # For RPM package for Centos 7; not tested
    cpack -G DEB # For Ubuntu; not tested



