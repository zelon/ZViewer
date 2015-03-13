/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2007. 7. 20
*	StringToValue.h
*
*                                       http://www.wimy.com
*********************************************************************/
#pragma once

#include "OptionFile.h"

class StringToValue final {
  enum class ValueType {
    kInt,
    kBool,
    kString,
  };

public:
  explicit StringToValue(tstring & val, int * iValue)
  {
    _Init(val, iValue, ValueType::kInt);
  }

  explicit StringToValue(tstring & val, bool * bValue)
  {
    _Init(val, bValue, ValueType::kBool);
  }

  explicit StringToValue(tstring & val, tstring * bValue)
  {
    _Init(val, bValue, ValueType::kString);
  }

  void InsertValueToMap(iniMap & mapData);
  void InsertMapToValue(iniMap & mapData);
  const tstring& getString() const { return string_; }

protected:
  void _Init(const tstring& str, void * pData, const ValueType valueType) {
    string_ = str;
    value_type_ = valueType;
    data_ = pData;
  }

  void* data_;
  tstring string_;
  ValueType value_type_;
};

