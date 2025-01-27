//************************************************************************************************************************
// Signal.h
// Version 1.0 April, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <functional>
//#include <mutex>
#include <memory>
#include <map>


namespace corex {

using namespace std::placeholders;
using FunctionId = size_t;

template <typename ...Args>
class Signal
{
protected:
	using fn_t = std::function <void(Args ...args)>;
	// std::shared_ptr<std::recursive_mutex> _m; 			 =========> Useless mutex (No thread)
	FunctionId 					_guid{ 0 };
	std::map <FunctionId, fn_t> _delegates;

public:
	Signal 					() 								{}
	Signal 					(const Signal &d) 				{	_delegates = d._delegates; 										}
	Signal& operator = 		(const Signal &d) 				{	_delegates = d._delegates; return *this; 						}
	~Signal					()								{	clear();														}

	void operator()			(Args ...args) 					{	notify(args...); 												}
	operator bool() 										{	return _delegates.size() != 0; 									}
	Signal& operator = 		(fn_t fn) 						{	clear(); push_back(fn); return *this; 							}
	FunctionId push_back	(fn_t fn) 						{	_delegates.insert(std::make_pair(++_guid, fn)); return _guid;	}
	FunctionId operator+=	(fn_t fn) 						{	return push_back(fn); 											}
	Signal& operator-=		(FunctionId id) 				{	remove(id); return *this;		 								}
	void remove				(FunctionId ind) 				{	_delegates.erase(ind); 											}
	void clear				() 								{	_delegates.clear(); 											}
	inline void notify		(Args ...args) 					{	if (!_delegates.size()) return;
															 	auto it = _delegates.begin();
															 	while (it != _delegates.end()) {
																	auto curr = it++;
																	curr->second(args...);
																}
															}
};

}