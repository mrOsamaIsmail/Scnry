#ifndef SCNRY_TYPES_H
#define SCNRY_TYPES_H


#include <string>
#include <list>
#include <unordered_map>
#include <array>

#define prntln(x) std::cout<< x <<"\n" 

template <typename key,typename value>
using dictionary = std::unordered_map<key,value>;

template <typename type,std::size_t count>
using Array = std::array<type, count>;

using string = std::string;

template <typename T>
using list  = std::list<T>; 

using uint = unsigned int;

#endif