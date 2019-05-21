#include "demuxer.h"

#include "ts_packet.h"

size_t challenge::demuxer_t::consume(const uint8_t *data, size_t available_bytes, uint32_t pid, std::ostream &sink) {

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
