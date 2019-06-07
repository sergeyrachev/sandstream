#include "main.h"
#include "version.h"
#include "birthday.h"

#include "demuxer.h"
#include "callback_es.h"
#include "callback_es_factory.h"
#include "packetizer.h"

#include <iostream>
#include <fstream>
#include <vector>

// For fixed-width integer types
#include <cstdlib>
// For size_t, nullptr_t, ptrdiff_t
#include <cstddef>

class output_sink_t:public challenge::callback_es_t{
public:
    explicit output_sink_t(std::unique_ptr<std::ostream> sink):sink(std::move(sink)){

    }

    void on_data(challenge::const_buffer_t buffer) override{
        sink->write(reinterpret_cast<const char*>(buffer.data), buffer.size);
    }

private:
    std::unique_ptr<std::ostream> sink;
};

class elementary_stream_handler_t : public challenge::callback_es_factory_t{
public:
    explicit elementary_stream_handler_t( const std::string& prefix)
     : prefix(prefix){
    }

    std::unique_ptr<challenge::callback_es_t> create(uint8_t stream_type, uint16_t pid ) override {
        static const std::string extension(".bin");
        std::string filename = prefix + std::to_string(pid) + extension;
        std::unique_ptr<std::ostream> sink(new std::ofstream(filename, std::ios::binary));
        sink->exceptions(std::ios::badbit);
        return std::unique_ptr<challenge::callback_es_t>(new output_sink_t(std::move(sink)));
    }

private:
    std::string prefix;
};

int main(int argc, char *argv[]){
    std::cout <<  argv[0] << " :: " << version << " :: " << birthday << std::endl;
    const bool wrong_args_run = argc != 3;
    if(  wrong_args_run ){
        std::cout << "Usage: " << argv[0] << " file.ts(local path) " << "output_prefix" << std::endl;

        // An application run without any arguments is a correct case;
        const bool no_args_run = argc == 1;
        return no_args_run ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    const std::string input_filename(argv[1]);
    std::ifstream source(input_filename, std::ios::binary );
    if(!source){
        std::cerr << "Bad input file" << std::endl;
        return EXIT_FAILURE;
    }

    // Might throw if bit has been set before but we check that above
    source.exceptions( std::ios::badbit );

    const std::string output_prefix(argv[2]);
    static const std::size_t buffer_limit = 10 * 1024 * 1024;
    challenge::packetizer_t packetizer(buffer_limit);
    challenge::demuxer_t dmx( std::unique_ptr<challenge::callback_es_factory_t>{new elementary_stream_handler_t(output_prefix)});

    while (source) {
        try {
            auto buffer = packetizer.request();
            source.read(reinterpret_cast<char *>(buffer.data), buffer.size);
            buffer.size = source.gcount();
            packetizer.confirm(buffer);
            dmx.consume(packetizer);
        } catch (const std::ios::failure &io_error) {
            std::cerr << "Non-recoverable IO error happened: " << io_error.what() << " code: " << io_error.code().value() << " message: " << io_error.code().message();
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
