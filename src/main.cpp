#include "main.h"

#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <list>
#include <map>
#include <cassert>
#include <memory>
#include <algorithm>

class ts_packet_t{
    static const uint8_t ts_sync_byte = 0x47;
public:
    static const size_t ts_packet_size = 188;
public:
    ts_packet_t(const uint8_t* data, size_t available_bytes){
        assert(*data == ts_sync_byte);
        assert(available_bytes >= ts_packet_size);

        payload_unit_start_indicator = (data[1] & 0x40ul) == 0x40;
        pid = ((data[1] & 0x1Ful) << 8ul) + (data[2] & 0xFFul);
        uint8_t adaptation_field_control = ((data[3] & 0x30ul) >> 4ul);

        adaptation_field = nullptr;
        adaptation_field_size = 0;

        payload = data + 4;
        payload_size = ts_packet_size - 4;

        static const uint8_t payload_only = 0x01;
        static const uint8_t adaptation_field_only = 0x02;
        static const uint8_t adaptation_field_and_payload = 0x03;

        if(adaptation_field_and_payload == adaptation_field_control || adaptation_field_only == adaptation_field_control) {
            uint8_t adaptation_field_length = payload[0];

            payload += 1;
            payload_size -=1;

            adaptation_field = payload;
            adaptation_field_size = adaptation_field_length;

            payload += adaptation_field_length;
            payload_size -= adaptation_field_length;
        }
    }

    static size_t sync(const uint8_t* data, size_t available_bytes){
        size_t skiped_bytes = 0;
        while( skiped_bytes < available_bytes && *(data + skiped_bytes) != ts_sync_byte){
            skiped_bytes++;
        }
        return skiped_bytes;
    }

    bool payload_unit_start_indicator;
    uint32_t pid;

    const uint8_t* adaptation_field;
    uint32_t adaptation_field_size;

    const uint8_t* payload;
    uint32_t payload_size;
};

class pes_packet_t{
    static const uint8_t packet_start_code_prefix_length = 3;
    static const uint8_t stream_id_length = 1;
    static const uint8_t PES_packet_length_length = 2;
public:
    pes_packet_t(const uint8_t* data, size_t available_bytes){
        payload = nullptr;
        payload_size = 0;

        if(!parse_header(data, available_bytes)){
            storage.assign(data, data + available_bytes);
        }
    }

    void assign(const uint8_t* data, size_t available_bytes){
        if(!storage.empty()){
            std::copy(data, data + available_bytes, std::back_inserter(storage));
            if(parse_header(storage.data(), storage.size())){
                storage.clear();
            }
        } else {
            payload = data;
            payload_size = available_bytes;
        }

        if(pes_packet_length){
            payload_size = std::min(pes_packet_length, available_bytes);
            pes_packet_length -= payload_size;
        }
    }

    const uint8_t* payload;
    uint32_t payload_size;

private:
    bool parse_header(const uint8_t* data, size_t available_bytes){
        payload = nullptr;
        payload_size = 0;

        if(available_bytes < packet_start_code_prefix_length + PES_packet_length_length + stream_id_length){
            return false;
        }

        assert(data[0] == 0x00);
        assert(data[1] == 0x00);
        assert(data[2] == 0x01);

        stream_id = data[3];
        pes_packet_length = uint32_t(data[4] << 8ul) | uint32_t(data[5]);

        static const uint8_t program_stream_map = 0xbc;
        static const uint8_t private_stream_1 = 0xbd;
        static const uint8_t padding_stream = 0xbe;
        static const uint8_t private_stream_2 = 0xbf;
        static const uint8_t ecm_stream = 0xf0;
        static const uint8_t emm_stream = 0xf1;
        static const uint8_t dsmcc_stream = 0xf2;
        static const uint8_t iso_iec_13522_stream = 0xf3;
        static const uint8_t h222_type_a_stream = 0xf4;
        static const uint8_t h222_type_b_stream = 0xf5;
        static const uint8_t h222_type_c_stream = 0xf6;
        static const uint8_t h222_type_d_stream = 0xf7;
        static const uint8_t h222_type_e_stream = 0xf8;
        static const uint8_t ancillary_stream = 0xf9;
        static const uint8_t sl_packetized_stream = 0xfa;
        static const uint8_t flexmux_stream = 0xfb;
        static const uint8_t reserved_data_stream_0 = 0xfc;
        static const uint8_t reserved_data_stream_1 = 0xfd;
        static const uint8_t reserved_data_stream_2 = 0xfe;
        static const uint8_t program_stream_directory = 0xFF;

        if( stream_id != program_stream_map &&
            stream_id != padding_stream &&
            stream_id != private_stream_2 &&
            stream_id != ecm_stream &&
            stream_id != emm_stream &&
            stream_id != program_stream_directory &&
            stream_id != dsmcc_stream &&
            stream_id != h222_type_e_stream ){

            if(available_bytes < 9){
                return false;
            }

            uint8_t pes_header_data_length = data[8];
            static const uint8_t flags_length = 3;
            size_t consumed_bytes = flags_length;

            if(available_bytes <  packet_start_code_prefix_length + PES_packet_length_length + stream_id_length + consumed_bytes + pes_header_data_length){
                return false;
            }

            consumed_bytes += pes_header_data_length;

            size_t payload_offset = packet_start_code_prefix_length + PES_packet_length_length + stream_id_length + consumed_bytes;

            payload = data + payload_offset;
            payload_size = available_bytes - payload_offset;

            if(pes_packet_length){
                pes_packet_length -= (pes_header_data_length + flags_length);
            }

        } else if( stream_id == program_stream_map ||
                 stream_id == private_stream_2 ||
                 stream_id == ecm_stream ||
                 stream_id == emm_stream ||
                 stream_id == program_stream_directory ||
                 stream_id == dsmcc_stream ||
                 stream_id == h222_type_e_stream ){

            payload = data;
            payload_size = available_bytes;

        } else if(stream_id == padding_stream){

            payload = data;
            payload_size = available_bytes;
        }

        return true;
    }

private:
    std::vector<uint8_t> storage{};

    uint8_t stream_id{};
    size_t pes_packet_length{};
};

class detector_t{
public:
    detector_t() = default;

    size_t detect(const uint8_t* data, size_t available_bytes){
        size_t consumed_bytes = 0;
        consumed_bytes += ts_packet_t::sync(data, available_bytes);

        if(available_bytes - consumed_bytes < ts_packet_t::ts_packet_size){
            return 0;
        }

        ts_packet_t packet(data + consumed_bytes, available_bytes - consumed_bytes);

        static const uint32_t pid_pat = 0;
        if(packet.payload_unit_start_indicator){
            if(packet.pid == pid_pat){
                parse_pat(packet.payload + 1, packet.payload_size - 1);
            } else if(packet.pid == pmt_pid){
                parse_pmt(packet.payload + 1, packet.payload_size - 1);
            }
        }
        consumed_bytes += ts_packet_t::ts_packet_size;
        return consumed_bytes;
    }

    std::vector<uint32_t> elementary_stream_pids{};

private:
    void parse_pat(const uint8_t* data, size_t available_bytes){
        static const size_t section_header_length = 3;
        static const size_t crc_length = 4;
        size_t consumed_bytes = 0;
        consumed_bytes += 1;

        uint32_t section_length = (data[consumed_bytes] & 0x0Ful) << 8ul | data[consumed_bytes + 1];
        consumed_bytes += 2;

        assert(section_length + section_header_length <= available_bytes);

        consumed_bytes += 5;
        while(consumed_bytes < section_length + section_header_length - crc_length){

            uint32_t program_number = uint32_t(data[consumed_bytes] << 8ul) | data[consumed_bytes + 1];
            consumed_bytes += 2;

            pmt_pid = (data[consumed_bytes] & 0x1Ful) << 8ul | data[consumed_bytes + 1];
            consumed_bytes += 2;
        }
    }
    void parse_pmt(const uint8_t* data, size_t available_bytes){

        elementary_stream_pids.clear();

        static const size_t section_header_length = 3;
        static const size_t crc_length = 4;
        size_t consumed_bytes = 0;
        consumed_bytes += 1;

        uint32_t section_length = (data[consumed_bytes] & 0x0Ful) << 8ul | data[consumed_bytes + 1];
        consumed_bytes += 2;

        assert(section_length + section_header_length <= available_bytes);
        consumed_bytes += 7;

        uint32_t program_info_length = (data[consumed_bytes] & 0x0Ful) << 8ul | data[consumed_bytes + 1];
        consumed_bytes += 2;
        consumed_bytes += program_info_length;

        while(consumed_bytes < section_length + section_header_length - crc_length){

            uint32_t  stream_type = data[consumed_bytes] ;
            consumed_bytes += 1;

            uint32_t elementary_pid = (data[consumed_bytes] & 0x1Ful)<< 8ul | data[consumed_bytes + 1];
            consumed_bytes += 2;

            uint32_t es_info_length = (data[consumed_bytes] & 0x0Ful) << 8ul | data[consumed_bytes + 1];
            consumed_bytes += 2;
            consumed_bytes += es_info_length;

            elementary_stream_pids.push_back(elementary_pid);
        }
    }
    uint32_t pmt_pid{0};
};

class demuxer_t{
public:
    demuxer_t() = default;

    size_t consume(const uint8_t* data, size_t available_bytes, uint32_t pid, std::ostream& sink){

        size_t consumed_bytes = 0;
        consumed_bytes += ts_packet_t::sync(data, available_bytes);

        if(available_bytes - consumed_bytes < ts_packet_t::ts_packet_size){
            return 0;
        }

        ts_packet_t packet(data + consumed_bytes, available_bytes - consumed_bytes);
        if(packet.pid == pid){
            if(packet.payload_unit_start_indicator){
                stream = std::unique_ptr<pes_packet_t>(new pes_packet_t(packet.payload, packet.payload_size));
                sink.write(reinterpret_cast<const char*>(stream->payload), stream->payload_size);
            } else if(stream){
                stream->assign(packet.payload, packet.payload_size);
                sink.write(reinterpret_cast<const char*>(stream->payload), stream->payload_size);
            }
        }
        consumed_bytes += ts_packet_t::ts_packet_size;
        return consumed_bytes;
    }

private:
    std::unique_ptr<pes_packet_t> stream;
};

int main(int argc, char *argv[]){
    if(argc == 1 || argc > 4){
        std::cout << "Usage: " << argv[0] << "file.ts(local path)" << "[ pid(integer) " << " output.bin(path)]" << std::endl;
        return 0;
    }

    const std::string input_filename(argv[1]);
    std::ifstream source(input_filename, std::ios::binary );

    if(argc == 2){

        detector_t detect;
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
            std::cout << "Elementary stream with pid found: " << es_pid << std::endl;
        }

        return 0;
    }

    if(argc == 4){
        const uint32_t pid(std::stol(argv[2], nullptr, 0));
        const std::string output_filename(argv[3]);

        std::ofstream sink(output_filename, std::ios::binary);

        demuxer_t dmx;
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
