#include "demuxer.h"

#include "ts_packet.h"
#include "parser_pat.h"
#include "parser_pmt.h"
#include "parser_pes.h"

challenge::demuxer_t::demuxer_t(std::unique_ptr<callback_es_factory_t> es_callback_factory)
: es_callback_factory(std::move(es_callback_factory)) {
    static const uint16_t pat_pid = 0x0000;
    packet_streams.emplace(pat_pid, std::unique_ptr<callback_ts_packet>{new parser_pat_t(*this)});
}

void challenge::demuxer_t::consume(std::unique_ptr<ts_packet_t> packet) {
    auto stream = packet_streams.find(packet->pid);
    if(stream != packet_streams.end()){
        stream->second->put(std::move(packet));
    }
}

void challenge::demuxer_t::on_pat(const pat_t &update) {
    if(services != update){
        services = update;
        for (const auto &service : services) {
            const auto pid = service.first;
            if(packet_streams.find(pid) == packet_streams.end()){
                packet_streams.emplace(pid, std::unique_ptr<callback_ts_packet>{new parser_pmt_t(*this)});
            }
        }
    }
}

void challenge::demuxer_t::on_pmt(const pmt_t &update) {
    for (const auto &track : update) {
        if(tracks.find(track.first) == tracks.end()){
            auto consumer = es_callback_factory->create(track.second, track.first);
            packet_streams.emplace(track.first, std::unique_ptr<callback_ts_packet>{new parser_pes_t(std::move(consumer))});
            tracks.insert(track);
        }
    }
}
