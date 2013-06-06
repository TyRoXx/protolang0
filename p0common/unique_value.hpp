#pragma once
#ifndef P0COMMON_UNIQUE_VALUE_HPP
#define P0COMMON_UNIQUE_VALUE_HPP


#include <utility>
#include <tuple>


namespace p0
{
	template <class Value, class ValuePolicy>
	struct unique_value : private ValuePolicy
	{
		typedef typename ValuePolicy::storage_type storage_type;

		unique_value()
		{
			ValuePolicy::acquire(m_value);
		}

		template <class V>
		explicit unique_value(V &&value)
			: m_value(std::forward<V>(value))
		{
			ValuePolicy::acquire(m_value);
		}

		template <class V, class P>
		explicit unique_value(V &&value, P &&policy)
			: ValuePolicy(std::forward<P>(policy))
			, m_value(std::forward<V>(value))
		{
			ValuePolicy::acquire(m_value);
		}

		unique_value(unique_value &&other)
			: ValuePolicy(std::move(static_cast<ValuePolicy &>(other)))
			, m_value(std::move(other.m_value))
		{
		}

		unique_value &operator = (unique_value &&other)
		{
			swap(other);
			unique_value().swap(other);
			return *this;
		}

		~unique_value()
		{
			ValuePolicy::release(m_value);
		}

		void swap(unique_value &other)
		{
			auto left = std::tie(
						static_cast<ValuePolicy &>(*this),
						m_value);
			auto right = std::tie(
						static_cast<ValuePolicy &>(other),
						other.m_value);
			left.swap(right);
		}

		Value const &operator * () const
		{
			return ValuePolicy::get_reference(m_value);
		}

		Value const *operator -> () const
		{
			return &ValuePolicy::get_reference(m_value);
		}

		//TODO: ==, !=, <, .., std::hash

	private:

		storage_type m_value;
	};
}


#endif
