/* 
 *
 * TinySHA1 - a header only implementation of the SHA1 algorithm in C++. Based
 * on the implementation in boost::uuid::details.
 * 
 * SHA1 Wikipedia Page: http://en.wikipedia.org/wiki/SHA-1
 * 
 * Copyright (c) 2012-22 SAURAV MOHAPATRA <mohaps@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include "../file/file.h"
#include "../util/linux.h"

namespace slisc {
	namespace sha1
	{
		class SHA1
		{
		public:
			typedef uint32_t digest32_t[5];
			typedef uint8_t digest8_t[20];
			inline static uint32_t LeftRotate(uint32_t value, size_t count) {
				return (value << count) ^ (value >> (32-count));
			}
			SHA1(){ reset(); }
			virtual ~SHA1() {}
			SHA1(const SHA1& s) { *this = s; }
			const SHA1& operator = (const SHA1& s) {
				memcpy(m_digest, s.m_digest, 5 * sizeof(uint32_t));
				memcpy(m_block, s.m_block, 64);
				m_blockByteIndex = s.m_blockByteIndex;
				m_byteCount = s.m_byteCount;
				return *this;
			}
			SHA1& reset() {
				m_digest[0] = 0x67452301;
				m_digest[1] = 0xEFCDAB89;
				m_digest[2] = 0x98BADCFE;
				m_digest[3] = 0x10325476;
				m_digest[4] = 0xC3D2E1F0;
				m_blockByteIndex = 0;
				m_byteCount = 0;
				return *this;
			}
			SHA1& processByte(uint8_t octet) {
				this->m_block[this->m_blockByteIndex++] = octet;
				++this->m_byteCount;
				if(m_blockByteIndex == 64) {
					this->m_blockByteIndex = 0;
					processBlock();
				}
				return *this;
			}
			SHA1& processBlock(const void* const start, const void* const end) {
				const uint8_t* begin = static_cast<const uint8_t*>(start);
				const uint8_t* finish = static_cast<const uint8_t*>(end);
				while(begin != finish) {
					processByte(*begin);
					begin++;
				}
				return *this;
			}
			SHA1& processBytes(const void* const data, size_t len) {
				const uint8_t* block = static_cast<const uint8_t*>(data);
				processBlock(block, block + len);
				return *this;
			}
			const uint32_t* getDigest(digest32_t digest) {
				size_t bitCount = this->m_byteCount * 8;
				processByte(0x80);
				if (this->m_blockByteIndex > 56) {
					while (m_blockByteIndex != 0) {
						processByte(0);
					}
					while (m_blockByteIndex < 56) {
						processByte(0);
					}
				} else {
					while (m_blockByteIndex < 56) {
						processByte(0);
					}
				}
				processByte(0);
				processByte(0);
				processByte(0);
				processByte(0);
				processByte( static_cast<unsigned char>((bitCount>>24) & 0xFF));
				processByte( static_cast<unsigned char>((bitCount>>16) & 0xFF));
				processByte( static_cast<unsigned char>((bitCount>>8 ) & 0xFF));
				processByte( static_cast<unsigned char>((bitCount)     & 0xFF));
		
				memcpy(digest, m_digest, 5 * sizeof(uint32_t));
				return digest;
			}
			const uint8_t* getDigestBytes(digest8_t digest) {
				digest32_t d32;
				getDigest(d32);
				size_t di = 0;
				digest[di++] = ((d32[0] >> 24) & 0xFF);
				digest[di++] = ((d32[0] >> 16) & 0xFF);
				digest[di++] = ((d32[0] >> 8) & 0xFF);
				digest[di++] = ((d32[0]) & 0xFF);
				
				digest[di++] = ((d32[1] >> 24) & 0xFF);
				digest[di++] = ((d32[1] >> 16) & 0xFF);
				digest[di++] = ((d32[1] >> 8) & 0xFF);
				digest[di++] = ((d32[1]) & 0xFF);
				
				digest[di++] = ((d32[2] >> 24) & 0xFF);
				digest[di++] = ((d32[2] >> 16) & 0xFF);
				digest[di++] = ((d32[2] >> 8) & 0xFF);
				digest[di++] = ((d32[2]) & 0xFF);
				
				digest[di++] = ((d32[3] >> 24) & 0xFF);
				digest[di++] = ((d32[3] >> 16) & 0xFF);
				digest[di++] = ((d32[3] >> 8) & 0xFF);
				digest[di++] = ((d32[3]) & 0xFF);
				
				digest[di++] = ((d32[4] >> 24) & 0xFF);
				digest[di++] = ((d32[4] >> 16) & 0xFF);
				digest[di++] = ((d32[4] >> 8) & 0xFF);
				digest[di++] = ((d32[4]) & 0xFF);
				return digest;
			}
		
		protected:
			void processBlock() {
				uint32_t w[80];
				for (size_t i = 0; i < 16; i++) {
					w[i]  = (m_block[i*4 + 0] << 24);
					w[i] |= (m_block[i*4 + 1] << 16);
					w[i] |= (m_block[i*4 + 2] << 8);
					w[i] |= (m_block[i*4 + 3]);
				}
				for (size_t i = 16; i < 80; i++) {
					w[i] = LeftRotate((w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]), 1);
				}
		
				uint32_t a = m_digest[0];
				uint32_t b = m_digest[1];
				uint32_t c = m_digest[2];
				uint32_t d = m_digest[3];
				uint32_t e = m_digest[4];
		
				for (std::size_t i=0; i<80; ++i) {
					uint32_t f = 0;
					uint32_t k = 0;
		
					if (i<20) {
						f = (b & c) | (~b & d);
						k = 0x5A827999;
					} else if (i<40) {
						f = b ^ c ^ d;
						k = 0x6ED9EBA1;
					} else if (i<60) {
						f = (b & c) | (b & d) | (c & d);
						k = 0x8F1BBCDC;
					} else {
						f = b ^ c ^ d;
						k = 0xCA62C1D6;
					}
					uint32_t temp = LeftRotate(a, 5) + f + e + k + w[i];
					e = d;
					d = c;
					c = LeftRotate(b, 30);
					b = a;
					a = temp;
				}
		
				m_digest[0] += a;
				m_digest[1] += b;
				m_digest[2] += c;
				m_digest[3] += d;
				m_digest[4] += e;
			}
		private:
			digest32_t m_digest;
			uint8_t m_block[64];
			size_t m_blockByteIndex;
			size_t m_byteCount;
		};
	} // namespace sha1

	// sha1sum for a block of data
	inline Str sha1sum(const char *p, Long_I N) {
		sha1::SHA1 s;
		s.processBytes(p, N);
		uint32_t digest[5];
		s.getDigest(digest);    
		char str[48];
		snprintf(str, 45, "%08x%08x%08x%08x%08x", digest[0], digest[1], digest[2], digest[3], digest[4]);
		return str;
	}

	// sha1sum for string
	inline Str sha1sum(Str_I str)  {
		return sha1sum(str.c_str(), str.size());
	}

	// sha1sum for file
	// needs RAM as large as the file!
	inline Str sha1sum_f(Str_I fname) {
		static thread_local Str str;
		read(str, fname);
		return sha1sum(str);
	}

	// use sha1sum in the command line
#if defined(SLS_USE_LINUX) || defined(SLS_USE_MACOS) || defined(SLS_USE_MINGW)
	inline Str sha1sum_f_exec(Str_I fname) {
		Str my_stdout;
		if (exec_str(my_stdout, "sha1sum \"" + fname + "\""))
			SLS_ERR("exec_str returned none-zero!");
		if (my_stdout.size() <= 41)
			SLS_ERR("exec_str() illegal output: " + my_stdout);
		return my_stdout.substr(0, 40);
	}
#endif

	// takes 100 bytes from 10 parts of the file then calculate SHA1
	// if the file size is less than 1000 bytes, calculate SHA1 for the whole file
	inline Str sha1sum_f_sample(Str_I file)
	{
		const Long numSegments = 10;
		const Long segmentSize = 100;
		static thread_local Str segment(segmentSize, '\0'), all_segments;

		ifstream fin(file, std::ifstream::binary);
		if (!fin)
			SLS_ERR("Cannot open file: " + file);

		// Determine the size of the file
		fin.seekg(0, fin.end);
		Long fileSize = fin.tellg();
		fin.seekg(0, fin.beg);
		Long step = fileSize / numSegments;

		if (fileSize < numSegments * segmentSize)
			return sha1sum_f(file);

		// read segments from the file
		all_segments.clear();
		for (Long i = 0; i < numSegments; ++i) {
			fin.seekg(i * step);
			fin.read(&segment[0], segmentSize);
			all_segments += segment;
		}
		return sha1sum(all_segments);
	}

} // namespace slisc
