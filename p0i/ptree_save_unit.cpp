#include "ptree_save_unit.hpp"
#include "unit.hpp"


namespace p0
{
	namespace intermediate
	{
		void save_unit(
			boost::property_tree::ptree &tree,
			unit const &unit
			)
		{
			tree.add("integer-width", unit.integer_width());
		}
	}
}
