#pragma once
#include <define.h>
#include <chrono>
#include <stream_buffer.hpp>


typedef enum log_level : uint8_t
{
	LLV_TRACE,
	LLV_DEBUG,
	LLV_INFO,
	LLV_WARNING,
	LLV_ERROR,
	LLV_FATAL
}log_level;

extern "C"
{
	EXPORT_FLAG unsigned char* alloc_log_buffer();
	
	EXPORT_FLAG void free_log_buffer(unsigned char*& dataptr);

	EXPORT_FLAG void log_print(log_level lv, const char* file, char const* func, uint32_t line, unsigned char* msg_data);

	EXPORT_FLAG void log_profile(log_level lv, const char* file, char const* func, uint32_t line, const char* msg);
}



class logline 
{
	unsigned char* _buffer ;
	stream_buffer _sd;
	
	log_level _lv;
	const char* _file;
	char const* _func;
	uint32_t _line;

public:


	logline(log_level lv, const char* file, char const* func, uint32_t line) :
		_lv(lv),
		_file(file),
		_func(func),
		_line(line),
		_buffer(alloc_log_buffer()),
		_sd(_buffer, 1024)
	{
		_sd.clear();
	}

	template <typename Frist, typename... Types>
	typename std::enable_if < !std::is_enum <Frist>::value, void >::type
		print(Frist firstArg, Types... args) {
		_sd <<static_cast<std::decay_t<Frist>>(firstArg)<<" ";
		print(args...);
	}
	template <typename Frist, typename... Types>
	typename std::enable_if < std::is_enum <Frist>::value, void >::type
		print(Frist firstArg, Types... args) {
		_sd << static_cast<uint8_t>(firstArg) << " ";
		print(args...);
	}
	template <typename... Types>
	void print(const std::string& firstArg, Types... args) {
		_sd << firstArg.c_str() << " ";
		print(args...);
	}
	template <typename... Types>
	void print(char* firstArg, Types... args) {
		_sd << static_cast<const char*>(firstArg) << " ";
		print(args...);
	}
	void print()
	{
		log_print(_lv, _file, _func, _line,_buffer);
	}
	
};


#ifndef NDEBUG
#define LOG_TRACE(...) logline(LLV_TRACE,__FILE__,__func__,__LINE__).print(__VA_ARGS__);
#define LOG_DEBUG(...) logline(LLV_DEBUG,__FILE__,__func__,__LINE__).print(__VA_ARGS__);
#else
#define LOG_TRACE(...) 
#define LOG_DEBUG(...) 
#endif
#define LOG_INFO(...) logline(LLV_INFO,__FILE__,__func__,__LINE__).print(__VA_ARGS__);
#define LOG_WARNING(...) logline(LLV_WARNING,__FILE__,__func__,__LINE__).print(__VA_ARGS__);
#define LOG_ERROR(...) logline(LLV_ERROR,__FILE__,__func__,__LINE__).print(__VA_ARGS__);
#define LOG_FATAL(...) logline(LLV_FATAL,__FILE__,__func__,__LINE__).print(__VA_ARGS__);

#define LOG_PROFILE(msg) //log_profile(LLV_FATAL,__FILE__,__func__,__LINE__,msg);
