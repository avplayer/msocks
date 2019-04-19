//
// Created by maxtorm on 2019/4/19.
//

#pragma once
#include <map>
#include <msocks/utility/socks_erorr.hpp>
#include <boost/system/system_error.hpp>
#include <botan/md5.h>

namespace shadowsocks
{
enum cipher_type
{
	STREAM = 0,
	AEAD = 1
};

namespace detail
{

struct cipher_info
{
	std::size_t key_length = 0;
	std::size_t iv_length = 0;
	std::string botan_name;
	cipher_type type = STREAM;
	// below is for AEAD cipher
	std::size_t salt_length = 0;
	std::size_t tag_length = 0;
};

const static std::map<std::string,cipher_info> supported_cipher
	{
		{"chacha20",{32,8,"ChaCha(20)",STREAM}},
		{"salsa20",{32,8,"",STREAM}},
		{"rc4-md5",{16,16,"",STREAM}},
		{"aes-128-ctr",{16,16,"",STREAM}},
		{"aes-192-ctr",{24,16,"",STREAM}},
		{"aes-256-ctr",{32,16,"",STREAM}},
		{"camellia-128-cfb",{16,16,"",STREAM}},
		{"camellia-192-cfb",{24,16,"",STREAM}},
		{"chacha20-ietf",{32,12,"",STREAM}},
		{"chacha20-ietf-poly1305",{32,12,"",AEAD,32,16}},
		{"aes-256-gcm",{32,12,"",AEAD,32,16}},
		{"aes-192-gcm",{24,12,"",AEAD,24,16}},
		{"aes-128-gcm",{16,12,"",AEAD,16,16}}
	};

}

class cipher_info
{
private:
	detail::cipher_info info_;
	std::vector<uint8_t> key_;
	
	
	static std::vector<uint8_t> EVP_BytesToKey(const std::string& password)
	{
		std::vector<std::vector<uint8_t>> m;
		std::vector<uint8_t> password1(password.begin(), password.end());
		std::vector<uint8_t> data;
		Botan::MD5 md5;
		int i = 0;
		while (m.size() < 32 + 8)
		{
			if (i == 0)
			{
				data = password1;
			}
			else
			{
				data = m[i - 1];
				std::copy(password1.begin(), password1.end(), std::back_inserter(data));
			}
			i++;
			auto hash_result = md5.process(data.data(), data.size());
			m.emplace_back(hash_result.begin(), hash_result.end());
		}
		std::vector<uint8_t> key;
		for (auto& mh : m)
		{
			std::copy(mh.begin(), mh.end(), std::back_inserter(key));
		}
		return std::vector<uint8_t>(key.begin(), key.begin() + 32);
	}
	
public:
	
	explicit cipher_info(const std::string &name,const std::string &password)
	{
		key_ = EVP_BytesToKey(password);
		if (detail::supported_cipher.find(name) == detail::supported_cipher.end())
		{
			boost::system::system_error(msocks::errc::cipher_algo_not_found, msocks::socks_category());
		}
		info_ = detail::supported_cipher.at(name);
	}
	
	std::vector<uint8_t> key() const
	{
		return key_;
	}
	
	std::size_t iv_length() const
	{
		return info_.iv_length;
	}
	
	std::size_t key_length() const
	{
		return info_.key_length;
	}
	
	const std::string &botan_name() const
	{
		return info_.botan_name;
	}
	
	std::size_t salt_length() const
	{
		return info_.salt_length;
	}
	
	std::size_t tag_length() const
	{
		return info_.tag_length;
	}
	
	cipher_type type() const
	
	{
		return info_.type;
	}
	
};

}

