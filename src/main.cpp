#include "main.h"
#include "version.h"
#include "birthday.h"

#include "demuxer.h"
#include "detector.h"

#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char *argv[]){
    std::cout <<  argv[0] << " :: " << version << " :: " << birthday << std::endl;

    if(argc == 1 || argc > 4){
        std::cout << "Usage: " << argv[0] << " file.ts(local path) " << " [ pid(integer) " << " output.bin(path) ]" << std::endl;
        return 0;
    }

    const std::string input_filename(argv[1]);
    std::ifstream source(input_filename, std::ios::binary );

    if(argc == 2){

        challenge::detector_t detect;
        static const size_t buffer_limit = 10 * 1024 * 1024;

        std::vector<uint8_t> buffer(buffer_limit);
        std::vector<uint8_t> buffer_b(buffer_limit);
        size_t filled_bytes = 0;
        while(source && detect.elementary_stream_pids.empty()){

            source.read(reinterpret_cast<char*>(buffer.data() + filled_bytes), buffer.size() - filled_bytes);
            size_t available_bytes = source.gcount() + filled_bytes;

            size_t consumed_bytes = 0;
            size_t used_bytes = 0;

            while((used_bytes = detect.detect(buffer.data() + consumed_bytes, available_bytes - consumed_bytes)) > 0){
                consumed_bytes += used_bytes;
            }

            swap(buffer, buffer_b);
            filled_bytes = std::copy(buffer_b.data() + consumed_bytes, buffer_b.data() + available_bytes, buffer.data()) - buffer.data();
        }

        for (const auto &es_pid : detect.elementary_stream_pids) {
            std::cout << "Found an elementary stream with a pid: " << es_pid << std::endl;
        }

        return 0;
    }

    if(argc == 4){
        const uint32_t pid(std::stol(argv[2], nullptr, 0));
        const std::string output_filename(argv[3]);

        std::ofstream sink(output_filename, std::ios::binary);

        challenge::demuxer_t dmx;
        static const size_t buffer_limit = 10 * 1024 * 1024;
        std::vector<uint8_t> buffer(buffer_limit);
        std::vector<uint8_t> buffer_b(buffer_limit);
        size_t filled_bytes = 0;
        while(source){

            source.read(reinterpret_cast<char*>(buffer.data() + filled_bytes), buffer.size() - filled_bytes);
            size_t available_bytes = source.gcount() + filled_bytes;

            size_t consumed_bytes = 0;
            size_t used_bytes = 0;
            while( (used_bytes = dmx.consume(buffer.data() + consumed_bytes, available_bytes - consumed_bytes, pid, sink)) > 0 ){
                consumed_bytes += used_bytes;
            }

            swap(buffer, buffer_b);
            filled_bytes = std::copy(buffer_b.data() + consumed_bytes, buffer_b.data() + available_bytes, buffer.data()) - buffer.data();
        }
    }

    return 0;
}
