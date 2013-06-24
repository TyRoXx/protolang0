#pragma once
#ifndef P0RT_MODULE_CACHE_HPP
#define P0RT_MODULE_CACHE_HPP


#include "p0common/final.hpp"
#include "p0run/value.hpp"
#include <functional>
#include <map>
#include <vector>


namespace p0
{
	namespace run
	{
		struct garbage_collector;
		struct interpreter;
	}

	namespace rt
	{
		struct lazy_module PROTOLANG0_FINAL_CLASS
		{
			typedef std::function<run::value (run::garbage_collector &)> loader;

			run::value cached;
			loader load;

			lazy_module();
			explicit lazy_module(run::value const &already_usable);
			explicit lazy_module(loader delayed);
		};


		typedef std::function<void (std::string, lazy_module)> module_consumer;
		typedef std::function<bool (std::string const &, module_consumer const &)>
			module_file_decoder;

		struct module_loader PROTOLANG0_FINAL_CLASS
		{
			void register_file_decoder(module_file_decoder decoder);

			bool initialize_modules_from_file(
				std::string const &file_name,
				module_consumer const &handle_module);

		private:

			std::vector<module_file_decoder> m_decoders;
		};


		struct module_cache PROTOLANG0_FINAL_CLASS
		{
			void add_module(
				std::string name,
				lazy_module module);

			run::value get_module(
				run::garbage_collector &gc,
				std::string const &name);

		private:

			struct module_state
			{
				lazy_module module;
				bool is_being_loaded;
			};

			struct module_load_lock;

			typedef std::map<std::string, module_state> modules_by_name;


			modules_by_name m_modules;
		};


		std::function<run::value (p0::run::interpreter &, std::string const &)>
		make_import(module_cache &modules);
	}
}


#endif
