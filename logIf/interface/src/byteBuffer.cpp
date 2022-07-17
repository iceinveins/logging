#include "../include/byteBuffer.h"

namespace Logging
{

ByteBuffer::ByteBuffer(uint32_t size) : wpos(0), rpos(0), buf(size)
{

}
ByteBuffer::ByteBuffer(uint8_t* arr, uint32_t size) : wpos(0), rpos(0), buf(size)
{
	putBytes(arr, size);
}

uint32_t 
ByteBuffer::bytesRemaining() 
{
	return size() - rpos;
}

void 
ByteBuffer::clear() 
{
	rpos = 0;
	wpos = 0;
	buf.clear();
}

uint8_t* 
ByteBuffer::data() 
{
	return buf.data();
}

void 
ByteBuffer::resize(uint32_t newSize) 
{
	buf.resize(newSize);
	rpos = 0;
	wpos = 0;
}

uint32_t 
ByteBuffer::size() 
{
	return buf.size();
}


// Read Functions

uint8_t 
ByteBuffer::get() const 
{
	return read<uint8_t>();
}

void 
ByteBuffer::getBytes(uint8_t* buf, uint32_t len) const 
{
	for (uint32_t i = 0; i < len; i++) {
		buf[i] = read<uint8_t>();
	}
}

char 
ByteBuffer::getChar() const 
{
	return read<char>();
}

uint32_t 
ByteBuffer::getInt() const 
{
	return read<uint32_t>();
}

uint64_t 
ByteBuffer::getLong() const 
{
	return read<uint64_t>();
}

uint16_t
ByteBuffer::getShort() const 
{
	return read<uint16_t>();
}

std::string
ByteBuffer::getString() const 
{
    std::string ans;
    uint32_t sz = read<uint32_t>();
	for(uint32_t i = 0; i < sz; ++i)
    {
        ans += read<char>();
    }
    return ans;
}

// Write Functions

void 
ByteBuffer::put(uint8_t b) 
{
	append<uint8_t>(b);
}

void 
ByteBuffer::putBytes(uint8_t* b, uint32_t len) 
{
	for (uint32_t i = 0; i < len; i++)
		append<uint8_t>(b[i]);
}

void 
ByteBuffer::putChar(char value) 
{
	append<char>(value);
}

void 
ByteBuffer::putInt(uint32_t value) 
{
	append<uint32_t>(value);
}

void 
ByteBuffer::putLong(uint64_t value) 
{
	append<uint64_t>(value);
}

void 
ByteBuffer::putShort(uint16_t value) 
{
	append<uint16_t>(value);
}

void 
ByteBuffer::putString(const std::string& str) 
{
    append<uint32_t>(str.size());
    for(auto& ch : str)
    {
	    append<char>(ch);
    }
}

void 
ByteBuffer::printAH() 
{
	uint32_t length = buf.size();
	std::printf("ByteBuffer Length= %d ASCII & Hex Print\n", length);
	for (uint32_t i = 0; i < length; i++) 
	{
		std::printf("0x%02x ", buf[i]);
	}

	std::printf("\n");
	for (uint32_t i = 0; i < length; i++) 
	{
		std::printf("%c ", buf[i]);
	}

	std::printf("\n");
}

}