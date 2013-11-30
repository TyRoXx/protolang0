#include "default_garbage_collector.hpp"
#include "object.hpp"
#include <algorithm>
#include <functional>


namespace p0
{
	namespace run
	{
		struct default_garbage_collector::object_handle
		{
			object_handle()
			{
			}

			explicit object_handle(std::unique_ptr<char []> data)
				: m_data(std::move(data))
			{
			}

			object_handle(object_handle &&other)
			{
				swap(other);
			}

			~object_handle()
			{
				if (get())
				{
					get()->~object();
				}
			}

			object_handle &operator = (object_handle &&other)
			{
				swap(other);
				return *this;
			}

			object *get() const
			{
				return reinterpret_cast<object *>(m_data.get());
			}

			void swap(object_handle &other)
			{
				m_data.swap(other.m_data);
			}

		private:

			std::unique_ptr<char []> m_data;
		};


		default_garbage_collector::default_garbage_collector()
		{
		}

		default_garbage_collector::~default_garbage_collector()
		{
		}

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
			object_handle handle(remove_storage(
								 reinterpret_cast<char *>(&constructed)));
			m_objects.push_back(std::move(handle));
		}

		void default_garbage_collector::unmark()
		{
			for (auto i = m_objects.begin(); i != m_objects.end(); ++i)
			{
				i->get()->unmark();
			}
		}

		void default_garbage_collector::sweep(sweep_mode::type mode)
		{
			//do a full collection in any case for simplicity
			(void)mode;

			auto const is_marked =
				[](object_handle const &handle) -> bool
			{
				return handle.get()->is_marked();
			};

			//move all objects which are not marked to the end of the vector
			auto const new_end = std::partition(m_objects.begin(),
												m_objects.end(),
												is_marked);

			//free memory
			m_objects.erase(new_end, m_objects.end());
		}

		std::unique_ptr<char[]> default_garbage_collector::remove_storage(char *storage) PROTOLANG0_NOEXCEPT
		{
			auto const s = std::find_if(begin(m_storage),
										end(m_storage),
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
