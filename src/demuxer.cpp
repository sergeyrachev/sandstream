#include "demuxer.h"

#include "ts_packet.h"
#include "parser_pat.h"
#include "parser_pmt.h"
#include "parser_pes.h"

using challenge::callback_es_factory_t;
using challenge::source_packet_t;

challenge::demuxer_t::demuxer_t(std::unique_ptr<callback_es_factory_t> es_callback_factory)
: es_callback_factory(std::move(es_callback_factory)) {
    static const uint16_t pat_pid = 0x0000;
    packet_streams.emplace(pat_pid, std::unique_ptr<callback_ts_packet>{new parser_pat_t(*this)});
}

void challenge::demuxer_t::consume(challenge::source_packet_t& source) {
    while(!source.is_depleted()){
        auto packet = source.get();
        auto stream = packet_streams.find(packet.pid);
        if(stream != packet_streams.end()){
            stream->second->put(packet);
        }
    }

}

void challenge::demuxer_t::on_pat(const pat_t &update) {
    for (const auto &service : update) {
        const auto pid = service.first;
        if(packet_streams.find(pid) == packet_streams.end()){
            packet_streams.emplace(pid, std::unique_ptr<callback_ts_packet>{new parser_pmt_t(*this)});
        }
    }
}

void challenge::demuxer_t::on_pmt(const pmt_t &update) {
    for (const auto &track : update) {
        const auto pid = track.first;
        if(packet_streams.find(pid) == packet_streams.end()){
            auto consumer = es_callback_factory->create(track.second, pid);
            packet_streams.emplace(pid, std::unique_ptr<callback_ts_packet>{new parser_pes_t(std::move(consumer))});
        }
    }
}
