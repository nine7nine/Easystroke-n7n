#ifndef __JSON_H__
#define __JSON_H__

#include "var.h"

#include <boost/algorithm/string/predicate.hpp>

#include <json/json.h>

#include <list>
#include <map>
#include <set>
#include <vector>

#define STORE(value) do { \
        ::store(root[#value], value); \
    } while(false)

#define LOAD(value) do { \
        ::load(root[#value], value); \
    } while(false)

inline
void store(Json::Value& jsonValue, const double& value) {
    jsonValue = value;
}
inline
void load(Json::Value& jsonValue, double& value) {
    value = jsonValue.asDouble();
}

inline
void store(Json::Value& jsonValue, const bool& value) {
    jsonValue = value;
}
inline
void load(Json::Value& jsonValue, bool& value) {
    value = jsonValue.asBool();
}

inline
void store(Json::Value& jsonValue, const int& value) {
    jsonValue = value;
}
inline
void load(Json::Value& jsonValue, int& value) {
    value = jsonValue.asInt();
}

inline
void store(Json::Value& jsonValue, const unsigned int& value) {
    jsonValue = value;
}
inline
void load(Json::Value& jsonValue, unsigned int& value) {
    value = jsonValue.asUInt();
}

inline
void store(Json::Value& jsonValue, const unsigned short& value) {
    jsonValue = value;
}
inline
void load(Json::Value& jsonValue, unsigned short& value) {
    value = jsonValue.asUInt();
}

inline
void store(Json::Value& jsonValue, const std::string& value) {
    jsonValue = value;
}
inline
void load(Json::Value& jsonValue, std::string& value) {
    value = jsonValue.asString();
}

template < typename T >
void store(Json::Value& jsonValue, const std::list<T>& value) {
    for(auto& v : value) {
        Json::Value entry;
        store(entry, v);
        jsonValue.append(entry);
    }
}
template < typename T >
void load(Json::Value& jsonValue, std::list<T>& value) {
    value.clear();
    for(int i = 0; i < jsonValue.size(); i++) {
        T v;
        load(jsonValue[i], v);
        value.push_back(v);
    }
}

template < typename T1, typename T2 >
void store(Json::Value& jsonValue, const std::map<T1, T2>& value) {
    for(auto& kv : value) {
        Json::Value entry;
        store(entry["k"], kv.first);
        store(entry["v"], kv.second);
        jsonValue.append(entry);
    }
}
template < typename T1, typename T2 >
void load(Json::Value& jsonValue, std::map<T1, T2>& value) {
    value.clear();
    for(int i = 0; i < jsonValue.size(); i++) {
        T1 k;
        load(jsonValue[i]["k"], k);
        T2 v;
        load(jsonValue[i]["v"], v);
        value[k] = v;
    }
}

template < typename T >
void store(Json::Value& jsonValue, const std::set<T>& value) {
    for(auto& v : value) {
        Json::Value entry;
        store(entry, v);
        jsonValue.append(entry);
    }
}
template < typename T >
void load(Json::Value& jsonValue, std::set<T>& value) {
    value.clear();
    for(int i = 0; i < jsonValue.size(); i++) {
        T v;
        load(jsonValue[i], v);
        value.insert(v);
    }
}

template < typename T >
void store(Json::Value& jsonValue, const std::vector<T>& value) {
    for(auto& v : value) {
        Json::Value entry;
        store(entry, v);
        jsonValue.append(entry);
    }
}
template < typename T >
void load(Json::Value& jsonValue, std::vector<T>& value) {
    value.clear();
    for(int i = 0; i < jsonValue.size(); i++) {
        T v;
        load(jsonValue[i], v);
        value.push_back(v);
    }
}

template < typename T >
void store(Json::Value& jsonValue, const boost::shared_ptr<T>& value) {
    if(!value) return;
    store(jsonValue, *value);
}
template < typename T >
void load(Json::Value& jsonValue, boost::shared_ptr<T>& value) {
    if(jsonValue.type() == Json::nullValue) return;
    value.reset(new T);
    load(jsonValue, *value.get());
}

template < typename T >
void store(Json::Value& jsonValue, const Source<T>& value) {
    store(jsonValue, value.ref());
}
template < typename T >
void load(Json::Value& jsonValue, Source<T>& value) {
    load(jsonValue, value.unsafe_ref());
}

#endif //__JSON_H__
