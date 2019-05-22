# sandstream

The coding challenge solution.

## Description

Naive Mpeg2 Transport Stream demultiplexer tool with functionality to detect all available elementary streams in MPEG2TS file. 
Run without arguments prints usage example. Run with media file as single argument prints out available elementary streams with their 
pid value that could be used in full commandline to demultiplex it. 

## End-User Information

Single elementary stream per run. Supports only PES-packetized elementary streams. Doesn't support stream format changes in stream or PAT/PMT updates.

### How-to use

To print all available elementary stream pid run:
    
    sandstream elephants.ts 
    
To get an elementary stream with pid equal 33 and write to file 33.bin use:
    
    sandstream elephants.ts 33 33.bin


## Dev Docs 

 IO level for input separated from main demuxer implementation to handle multiple IO backend. Demuxer handles one TS packet per call with returning 
 how many bytes consumed so there is no limit on input buffer granularity. IO can provide as many bytes as available without additional alignement per packet boundary. 
 Demuxer skips unneeded bytes and restore bytestream synchronization on sync_byte. During implementation extended Mpeg2TS formats taken into account as BDAV with 4 extra
 bytes prepending TS packet and/or  ATSC streams with FEC bytes in front of every packet. All these extra bytes silently skipped by demuxing on resync step.
 
 Extra data copiyng has been avoided so demultiplexing process happens on every packet.
 
 Elementary streams detection separated to do not overcomplicate demultiplexer implementation. This solution may need redesign and rework depends on use-cases and applications. 
   
 Naive internal architecture doesn't imply future reusage as is and has many trade-offs. A few growing points:

 - No generic table section parsing (currently only PAT/PMT stored in single TS Packet is supported)
 - Elementary stream consumer API should be designed better, currently use C++ IOStream as output
 - Better unittest compatibility with better source code modularity and granularity
     
 ### Tips and trick
 "version.h" and "birthday.h" provides a generic way to update internal application version and date of build. The values are printed then on run as 
    
    sandstream.exe :: 0.0.0+g0000000 :: 2019-05-22T08:08:39Z
   
### How-to build

    mkdir build && cd build
    cmake ..
    cmake --build . 


