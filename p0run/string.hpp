#pragma once
#ifndef P0RUN_STRING_HPP
#define P0RUN_STRING_HPP


#include "object.hpp"


namespace p0
{
	namespace run
	{
		struct string : object
		{
			explicit string(std::string content);
			virtual boost::optional<value> get_element(value const &key) const override;
			virtual bool set_element(value const &key, value const &value) override;

		private:

			std::string m_content;


			virtual void mark_recursively() override;
		};
	}
}


#endif
