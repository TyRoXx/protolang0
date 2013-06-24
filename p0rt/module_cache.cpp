#include "module_cache.hpp"
#include "p0run/interpreter.hpp"
#include <stdexcept>


namespace p0
{
	namespace rt
	{
		lazy_module::lazy_module()
		{
		}

		lazy_module::lazy_module(run::value const &already_usable)
			: cached(already_usable)
		{
		}

		lazy_module::lazy_module(loader delayed)
			: load(std::move(delayed))
		{
		}


		void module_loader::register_file_decoder(module_file_decoder decoder)
		{
			m_decoders.push_back(std::move(decoder));
		}

		bool module_loader::initialize_modules_from_file(
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


		struct module_cache::module_load_lock
		{
			explicit module_load_lock(module_state &state)
				: m_state(state)
			{
				if (m_state.is_being_loaded)
				{
					throw std::runtime_error(
								"Cyclic module dependency detected");
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

		void module_cache::add_module(
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

		run::value module_cache::get_module(
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
