
#include "stdafx.h"
#include "StringToValue.h"
#include "CommonFunc.h"

void StringToValue::InsertValueToMap(iniMap & mapData)
{
	switch ( m_valueType )
	{
	case eValueType_BOOL:
		{
			bool * pData = (bool*)m_pData;
			mapData[m_str] = (*pData) ? TEXT("true") : TEXT("false");

		}
		break;

	case eValueType_INT:
		{
			int * pData = (int*)m_pData;
			mapData[m_str] = toString(*pData);
		}
		break;
	default:
		assert(false);
	}
}


void StringToValue::InsertMapToValue(iniMap & mapData)
{
	switch ( m_valueType )
	{
	case eValueType_BOOL:
		{
			iniMap::const_iterator it = mapData.find(m_str);

			if ( it == mapData.end() )
			{
				return;
			}

			const tstring value = it->second;

			bool * pData = (bool*)m_pData;
			*pData = (it->second == TEXT("true"));

		}
		break;

	case eValueType_INT:
		{
			iniMap::const_iterator it = mapData.find(m_str);

			if ( it == mapData.end() )
			{
				return;
			}

			const tstring value = it->second;

			int * pData = (int*)m_pData;
			*pData = _tstoi(value.c_str());
		}
		break;
	default:
		assert(false);
	}
}