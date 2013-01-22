#pragma once
#ifndef P0RUN_OBJECT_HPP
#define P0RUN_OBJECT_HPP


namespace p0
{
	namespace run
	{
		struct object
		{
			object();
			virtual ~object();
			void unmark();
			bool is_marked() const;
			virtual void mark_recursively() = 0;

		private:

			bool m_is_marked;
		};
	}
}


#endif
