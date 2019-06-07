#pragma once

#include "callback_es.h"
#include "ts_packet.h"
#include "callback_pat.h"
#include "callback_pmt.h"
#include "callback_es_factory.h"
#include "callback_ts_packet.h"
#include "source_packet.h"

#include <memory>
#include <map>

namespace challenge{

    ///
    /// @brief Demupltiplexer engine;
    /// User should provide @see callback_es_factory_t implementation to get elementary stream data into @see callback_es_t instance
    /// @see source_packet_t interface is provided by @see packetizer_t; demuxer gets packets from the interface
    ///
    class demuxer_t : public challenge::callback_pat_t, public challenge::callback_pmt_t{
    public:
        explicit demuxer_t(std::unique_ptr<challenge::callback_es_factory_t> es_callback_factory);
        void consume(challenge::source_packet_t& source);

    protected:
        void on_pat( const pat_t& update) override;
        void on_pmt(const pmt_t& update) override;

    private:
        std::unique_ptr<challenge::callback_es_factory_t> es_callback_factory;
        std::map<pid_t, std::unique_ptr<challenge::callback_ts_packet_t>> packet_streams;
    };
}


