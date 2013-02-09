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
			std::string const &content() const;
			virtual boost::optional<value> get_element(value const &key) const override;
			virtual bool set_element(value const &key, value const &value) override;
			virtual integer get_hash_code() const override;
			virtual bool equals(object const &other) const override;
			virtual comparison_result::Enum compare(object const &right) const override;

		private:

			std::string m_content;


			virtual void mark_recursively() override;
		};
	}
}


#endif
