#pragma once

#include <functional>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

#include "parsed_structs.h"

namespace explore {
	// base function declaration
	template<typename T>
	class base_function final : private boost::noncopyable {
	public:
		typedef void (signature)(T&&);
		typedef std::function<signature> function_type;
	private:
		function_type fn_ = [](T&&)->void {};

	public:
		void operator()(T &&data)const { fn_(std::forward<T>(data)); }
		void operator()(function_type &&fn) { fn_ = fn; }
	};

	// threadsafe base function declaration with shared mutex
	template<typename T>
	class base_threadsafe_function final : private boost::noncopyable {
	public:
		typedef void (signature)(T&&);
		typedef std::function<signature> function_type;
	private:
		function_type fn_ = [](T&&)->void {};
		boost::shared_mutex mtx;
	public:
		void operator()(T &&data)const { boost::shared_lock<boost::shared_mutex> lck(mtx); fn_(std::forward<T>(data)); }
		void operator()(function_type &&fn) { boost::unique_lock<boost::shared_mutex> lck(mtx); fn_ = fn; }
	};

	template<template <class> class _Base>
	class base_forward final : private boost::noncopyable {
	private:
		_Base<std::string> on_error_;
		_Base<sens_packet> on_sens_;
		_Base<eeg4_packet> on_eeg4_;
		_Base<eeg8_packet> on_eeg8_;
		_Base<acc_packet> on_acc_;
	public:

		template<typename _Var> void on_error(_Var &&v) { on_error_(std::forward<_Var>(v)); }
		template<typename _Var> void on_sens(_Var &&v) { on_sens_(std::forward<_Var>(v)); }
		template<typename _Var> void on_eeg4(_Var &&v) { on_eeg4_(std::forward<_Var>(v)); }
		template<typename _Var> void on_eeg8(_Var &&v) { on_eeg8_(std::forward<_Var>(v)); }
		template<typename _Var> void on_acc(_Var &&v) { on_acc_(std::forward<_Var>(v)); }
	};

	// simple skeleton sample forward class template
	class forward_skeleton {
	public:
		void on_error(std::string &&err) {}
		void on_sens(sens_packet &&data) {}
		void on_eeg4(eeg4_packet &&data) {}
		void on_eeg8(eeg8_packet &&data) {}
		void on_acc(acc_packet &&data) {}
	};
}
