#pragma once

#include "callback_es.h"
#include "ts_packet.h"
#include "callback_pat.h"
#include "callback_pmt.h"
#include "callback_es_factory.h"
#include "callback_ts_packet.h"

#include <memory>
#include <map>

namespace challenge{
    class demuxer_t : public callback_pat_t, public callback_pmt_t{
    public:
        explicit demuxer_t(std::unique_ptr<callback_es_factory_t> es_callback_factory);
        void consume(std::unique_ptr<ts_packet_t> packet);

    protected:
        void on_pat( const pat_t& update) override;
        void on_pmt(const pmt_t& update) override;

    private:
        std::unique_ptr<callback_es_factory_t> es_callback_factory;
        std::map<pid_t, std::unique_ptr<callback_ts_packet>> packet_streams;
        pat_t services;
        pmt_t tracks;
    };
}


