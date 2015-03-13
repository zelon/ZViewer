#include "stdafx.h"
#include "StringToValue.h"

#include "CommonFunc.h"

void StringToValue::InsertValueToMap(iniMap & mapData) {
  switch (value_type_) {
  case ValueType::kBool: {
    bool * pData = (bool*)data_;
    mapData[string_] = (*pData) ? TEXT("true") : TEXT("false");
    break;
  }

  case ValueType::kInt: {
    const int int_value = *(int*)data_;
    mapData[string_] = toString(int_value);
    break;
  }

  case ValueType::kString: {
    const tstring string_value = *(tstring*)data_;
    mapData[string_] = string_value;
    break;
  }
  default:
    assert(false);
  }
}


void StringToValue::InsertMapToValue(iniMap& mapData) {
  auto it = mapData.find(string_);
  if (it == mapData.end()) {
    return;
  }

  const tstring value = it->second;

  switch (value_type_) {
  case ValueType::kBool: {
    bool * pData = (bool*)data_;
    *pData = (it->second == TEXT("true"));

    break;
  }

  case ValueType::kInt: {
    int * pData = (int*)data_;
    *pData = _tstoi(value.c_str());
    break;
  }

  case ValueType::kString: {
    tstring * pData = (tstring*)data_;
    *pData = value;
    break;
  }

  default:
    assert(false);
  }
}