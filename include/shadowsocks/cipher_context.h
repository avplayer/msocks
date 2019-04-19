#pragma once

#include <botan/stream_cipher.h>
#include <botan/auto_rng.h>

#include <shadowsocks/cipher_context.h>
#include <shadowsocks/cipher_info.hpp>
#include  <msocks/utility/socks_erorr.hpp>

namespace shadowsocks
{

class cipher_context
{
    struct engine
    {
        std::unique_ptr<Botan::StreamCipher> cipher_;
        size_t iv_wanted_;
        std::vector<uint8_t> iv_;
    };

public:
    cipher_context(const cipher_info& info)
    {
        for(auto & i : engine_)
        {
            i.cipher_ = Botan::StreamCipher::create(info.botan_name());
            if(!i.cipher_)
            {
                throw boost::system::system_error(msocks::errc::cipher_algo_not_found, msocks::socks_category());
            }
            
            if(!i.cipher_->valid_keylength(info.key_length()))
            {
                throw boost::system::system_error(msocks::errc::cipher_keylength_invalid, msocks::socks_category());
            }
            i.cipher_->set_key(info.key());
            
            if(!i.cipher_->valid_iv_length(info.iv_length()))
            {
                throw boost::system::system_error(msocks::errc::cipher_ivlength_invalid, msocks::socks_category());
            }
            i.iv_wanted_ = info.iv_length();
            i.iv_.resize(info.iv_length());
        }
        Botan::AutoSeeded_RNG{}.randomize(engine_[1].iv_.data(), engine_[1].iv_.size());
    }

    std::array<engine, 2> engine_;
};

}
