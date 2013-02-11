#include "pretty_print_error.hpp"
#include "compiler_error.hpp"
#include <sstream>


namespace p0
{
	void pretty_print_error(
		std::ostream &out,
		p0::source_range source,
		p0::compiler_error const &error
		)
	{
		auto const pos = error.position();

		typedef std::reverse_iterator<p0::source_range::iterator> reverse_source_iterator;

		auto const begin_of_line = std::find(
			reverse_source_iterator(pos.begin()),
			reverse_source_iterator(source.begin()),
			'\n').base();

		auto const end_of_line = std::find(
			pos.begin(),
			source.end(),
			'\n');

		auto const max_hint_length = 72;
		auto const half_hint = (max_hint_length / 2);
		auto const hint_begin = (std::max)(pos.begin() - half_hint, begin_of_line);
		auto const hint_end   = (std::min)(pos.begin() + half_hint, end_of_line);

		std::string hint(
			hint_begin,
			hint_end
			);

		std::for_each(
			begin(hint),
			end(hint),
			[](char &c)
		{
			switch (c)
			{
			case '\r':
			case '\t':
				c = ' ';
				break;

			default:
				break;
			}
		});

		out << error.what() << '\n';

		auto const line_index = static_cast<size_t>(std::count(
			source.begin(),
			pos.begin(),
			'\n')); //TODO O(n) instead of O(n^2)

		std::ostringstream line_label_formatter;
		line_label_formatter << (line_index + 1) << ": ";
		auto const line_label = line_label_formatter.str();

		out << line_label << hint << '\n';

		auto const error_char_index =
			line_label.size() +
			static_cast<size_t>(std::distance(hint_begin, pos.begin()));

		out << std::string(error_char_index, ' ') << "^\n";
	}
}
