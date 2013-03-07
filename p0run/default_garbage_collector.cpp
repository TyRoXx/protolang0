#include "default_garbage_collector.hpp"
#include "object.hpp"
#include <algorithm>
#include <functional>


namespace p0
{
	namespace run
	{
		char *default_garbage_collector::allocate(std::size_t byte_size)
		{
			//assumption: new char[] returns memory which is aligned for any object
			//TODO: do this more portably with respect to alignment requirements
			std::unique_ptr<char []> allocated(new char[byte_size]);
			m_storage.push_back(std::move(allocated));
			return m_storage.back().get();
		}

		void default_garbage_collector::deallocate(char *storage)
		{
			remove_storage(storage);
		}

		void default_garbage_collector::commit_object(object &constructed)
		{
			try
			{
				m_objects.push_back(remove_storage(
									reinterpret_cast<char *>(&constructed)));
			}
			catch (...)
			{
				//TODO: make this responsibility explicit in constructed's type
				constructed.~object();
				throw;
			}
		}

		void default_garbage_collector::unmark()
		{
			for (auto i = m_objects.begin(); i != m_objects.end(); ++i)
			{
				reinterpret_cast<object *>(i->get())->unmark();
			}
		}

		void default_garbage_collector::sweep(sweep_mode::type mode)
		{
			//do a full collection in any case for simplicity
			(void)mode;

			auto const is_marked =
				[](std::unique_ptr<char []> const &memory) -> bool
			{
				return reinterpret_cast<object *>(memory.get())->is_marked();
			};

			//move all objects which are not marked to the end of the vector
			auto const new_end = std::partition(m_objects.begin(),
												m_objects.end(),
												is_marked);

			//call destructors
			std::for_each(new_end, m_objects.end(),
						  [](std::unique_ptr<char []> const &memory)
			{
				object * const obj = reinterpret_cast<object *>(memory.get());
				assert(obj);
				obj->~object();
			});

			//free memory
			m_objects.erase(new_end, m_objects.end());
		}

		std::unique_ptr<char []> default_garbage_collector::remove_storage(char *storage)
		{
			auto const s = std::find_if(begin(m_storage), end(m_storage),
						 [storage](std::unique_ptr<char []> const &element)
			{
						 return (element.get() == storage);
			});
			assert(s != end(m_storage));
			auto result = std::move(*s);
			swap(*s, m_storage.back());
			m_storage.pop_back();
			return result;
		}
	}
}
