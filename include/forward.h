#pragma once

#include <functional>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

#include "parsed_structs.h"

namespace explore {
	// base function declaration
	template<typename ... _Args>
	class base_function final 
		: private boost::noncopyable {
	public:
		typedef void (signature)(_Args&&...);
		typedef std::function<signature> function_type;
	private:
		function_type fn_ = [](_Args&&...)->void {};

	public:
		void operator()(_Args &&...data)const { fn_(std::forward<_Args>(data)...); }
		void operator()(function_type &&fn) { fn_ = fn; }
		void operator=(function_type &&fn) { fn_ = fn; }

	};

	// void parameter case (unused in libexplore)
	template<>
	class base_function<void> final
		: private boost::noncopyable {
	public:
		typedef void (signature)();
		typedef std::function<signature> function_type;
	private:
		function_type fn_ = []()->void {};

	public:
		void operator()()const { fn_(); }
		void operator()(function_type &&fn) { fn_ = fn; }
		void operator=(function_type &&fn) { fn_ = fn; }
	};

	// threadsafe base function declaration with shared mutex
	template<typename ... _Args>
	class base_threadsafe_function final 
		: private boost::noncopyable {
	public:
		typedef void (signature)(_Args&&...);
		typedef std::function<signature> function_type;
	private:
		function_type fn_ = [](_Args&&...)->void {};
		boost::shared_mutex mtx;
	public:
		void operator()(_Args&& ... data)const { boost::shared_lock<boost::shared_mutex> lck(mtx); fn_(std::forward<_Args>(data)...); }
		void operator()(function_type &&fn) { boost::unique_lock<boost::shared_mutex> lck(mtx); fn_ = fn; }
		void operator=(function_type &&fn) { boost::unique_lock<boost::shared_mutex> lck(mtx); fn_ = fn; }
	};

	template<template <class...> class _Base>
	class base_forward final 
		: private boost::noncopyable {
	public:
		_Base<bool> on_connect;
		_Base<std::string> on_error;
		_Base<sens_packet> on_sens;
		_Base<eeg4_packet> on_eeg4;
		_Base<eeg8_packet> on_eeg8;
		_Base<acc_packet> on_acc;
	};

	// simple skeleton sample forward class template
	class forward_skeleton {
	public:
		void on_connect(const bool b) {}
		void on_error(const std::string &err) {}
		void on_sens(sens_packet &&data) {}
		void on_eeg4(eeg4_packet &&data) {}
		void on_eeg8(eeg8_packet &&data) {}
		void on_acc(acc_packet &&data) {}
	};
}
