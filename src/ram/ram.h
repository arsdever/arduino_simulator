#pragma once

#include "ram_global.h"
#include <QByteArray>
#include <exception>

class RAM_EXPORT CRAM
{
public:
	CRAM(quint8* data, quint32 nSize);
	CRAM(quint32 nSize = 0x10000);
	~CRAM();

	template <typename INT_TYPE>
	INT_TYPE& operator[](qint32 address)
	{
		if (address + sizeof(INT_TYPE) > m_nSize)
		{
			throw index_out_of_bounds_exception();
		}

		return (INT_TYPE&)(*(quint8*)(m_pMemory + address));
	}

	quint32 GetSize() const;

private:
	quint32 m_nSize;
	quint8* m_pMemory;

public:
	struct ram_exception : public std::exception {};
	struct index_out_of_bounds_exception : ram_exception {};
};
