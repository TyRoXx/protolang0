#pragma once
#ifndef P0RUN_STRING_HPP
#define P0RUN_STRING_HPP


#include "object.hpp"
#include "p0common/final.hpp"
#include <string>


namespace p0
{
	namespace run
	{
		struct string : object
		{
			explicit string(std::string content);
			std::string const &content() const;
			virtual boost::optional<value> get_element(value const &key) const PROTOLANG0_FINAL_METHOD;
			virtual bool set_element(value const &key, value const &value) PROTOLANG0_FINAL_METHOD;
			virtual integer get_hash_code() const PROTOLANG0_FINAL_METHOD;
			virtual bool equals(object const &other) const PROTOLANG0_FINAL_METHOD;
			virtual comparison_result::Enum compare(object const &right) const PROTOLANG0_FINAL_METHOD;
			virtual void print(std::ostream &out) const PROTOLANG0_FINAL_METHOD;

		private:

			std::string m_content;


			virtual void mark_recursively() override;
		};
	}
}


#endif
