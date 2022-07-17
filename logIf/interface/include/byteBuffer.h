#pragma once
#include <string>
#include <string.h>
#include <vector>

namespace Logging
{
class ByteBuffer {
public:
    static constexpr int DEFAULT_SIZE = 4096;
	ByteBuffer(uint32_t size = DEFAULT_SIZE);
	ByteBuffer(uint8_t* arr, uint32_t size);
	~ByteBuffer() = default;

	uint32_t bytesRemaining();
	void clear();
    uint8_t* data();

	void resize(uint32_t newSize);
	uint32_t size();

	// Read
	uint8_t peek() const;
	uint8_t get() const;
	void getBytes(uint8_t* buf, uint32_t len) const;
	char getChar() const;
	uint32_t getInt() const;
	uint64_t getLong() const;
	uint16_t getShort() const;
	std::string getString() const;

	// Write
	void put(uint8_t b);
	void putBytes(uint8_t* b, uint32_t len);
	void putChar(char value);
	void putInt(uint32_t value);
	void putLong(uint64_t value);
	void putShort(uint16_t value);
    void putString(const std::string& str);

	void printAH();

private:
	uint32_t wpos;
	mutable uint32_t rpos;
	std::vector<uint8_t> buf;

	template<typename T> T read() const 
    {
		T data = read<T>(rpos);
		rpos += sizeof(T);
		return data;
	}

	template<typename T> T read(uint32_t index) const {
		if (index + sizeof(T) <= buf.size())
			return *((T*) &buf[index]);
		return 0;
	}

	template<typename T> void append(T data) 
    {
		uint32_t s = sizeof(data);

		if (size() < (wpos + s))
			buf.resize(wpos + s);
		memcpy(&buf[wpos], (uint8_t*) &data, s);
		wpos += s;
	}
};
}