#ifndef JSONCPP_H
#define JSONCPP_H

#include "jsoncpp/json.h"

#include<exception>
#include<string>

namespace Json{
  inline void RequireMember(const Json::Value& root){
  }
  template<typename ...T>
  inline void RequireMember(const Json::Value& root, std::string attribute, T... additional){
    if(!root.isMember(attribute)) throw std::runtime_error("attribute not found: "+attribute);
    RequireMember(root, additional...);
  }

}


#endif
