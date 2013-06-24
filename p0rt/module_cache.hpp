#pragma once
#ifndef P0RT_MODULE_CACHE_HPP
#define P0RT_MODULE_CACHE_HPP


#include "p0common/final.hpp"
#include "p0run/value.hpp"
#include "p0run/garbage_collector.hpp"
#include "p0run/interpreter.hpp"
#include <functional>
#include <map>
#include <vector>
#include <stdexcept>


namespace p0
{
	namespace rt
	{
		struct lazy_module PROTOLANG0_FINAL_CLASS
		{
			typedef std::function<run::value (run::garbage_collector &)> loader;

			run::value cached;
			loader load;

			lazy_module()
			{
			}

			explicit lazy_module(run::value const &already_usable)
				: cached(already_usable)
			{
			}

			explicit lazy_module(loader delayed)
				: load(std::move(delayed))
			{
			}
		};

		typedef std::function<void (std::string, lazy_module)> module_consumer;
		typedef std::function<bool (std::string const &, module_consumer const &)>
			module_file_decoder;

		struct module_loader PROTOLANG0_FINAL_CLASS
		{
			void register_file_decoder(module_file_decoder decoder)
			{
				m_decoders.push_back(std::move(decoder));
			}

			bool initialize_modules_from_file(
				std::string const &file_name,
				module_consumer const &handle_module)
			{
				for (auto f = begin(m_decoders); f != end(m_decoders); ++f)
				{
					auto const &decode = *f;
					if (decode(file_name, handle_module))
					{
						return true;
					}
				}
				return false;
			}

		private:

			std::vector<module_file_decoder> m_decoders;
		};

		struct module_cache PROTOLANG0_FINAL_CLASS
		{
			void add_module(
				std::string name,
				lazy_module module)
			{
				if (m_modules.find(name) != m_modules.end())
				{
					throw std::runtime_error(
						"The module name '" + name + "' is already in use");
				}

				m_modules.insert(std::make_pair(std::move(name),
												module_state{std::move(module), false}));
			}

			run::value get_module(
				run::garbage_collector &gc,
				std::string const &name)
			{
				auto const m = m_modules.find(name);
				if (m == m_modules.end())
				{
					return run::value();
				}

				auto &state = m->second;
				module_load_lock const lock(state);

				if (state.module.cached == run::value())
				{
					state.module.cached = state.module.load(gc);
				}

				return state.module.cached;
			}

		private:

			struct module_state
			{
				lazy_module module;
				bool is_being_loaded;
			};

			struct module_load_lock
			{
				explicit module_load_lock(module_state &state)
					: m_state(state)
				{
					if (m_state.is_being_loaded)
					{
						throw std::runtime_error(
									"Recursive module dependency detected");
					}

					m_state.is_being_loaded = true;
				}

				~module_load_lock()
				{
					m_state.is_being_loaded = false;
				}

			private:

				module_state &m_state;
			};

			typedef std::map<std::string, module_state> modules_by_name;


			modules_by_name m_modules;
		};

		std::function<run::value (p0::run::interpreter &, std::string const &)>
		make_import(module_cache &modules)
		{
			return [&modules](p0::run::interpreter &interpreter,
			                  std::string const &module_name)
			{
				return modules.get_module(interpreter.garbage_collector(),
			                              module_name);
			};
		}
	}
}


#endif
