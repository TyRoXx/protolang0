#pragma once
#ifndef P0COMMON_UNIQUE_VALUE_HPP
#define P0COMMON_UNIQUE_VALUE_HPP


#include <utility>
#include <tuple>


namespace p0
{
	template <class Value, class OwnershipPolicy>
	struct unique_value : private OwnershipPolicy
	{
		unique_value()
		{
		}

		template <class V>
		explicit unique_value(V &&value)
			: m_value(std::forward<V>(value))
		{
		}

		template <class V, class O>
		explicit unique_value(V &&value, O &&ownership)
			: OwnershipPolicy(std::forward<O>(ownership))
			, m_value(std::forward<V>(value))
		{
			OwnershipPolicy::acquire(m_value);
		}

		unique_value(unique_value &&other)
			: OwnershipPolicy(std::move(static_cast<OwnershipPolicy &>(other)))
			, m_value(std::move(other.m_value))
		{
			OwnershipPolicy::acquire(m_value);
		}

		unique_value &operator = (unique_value &&other)
		{
			swap(other);
			unique_value().swap(other);
			return *this;
		}

		~unique_value()
		{
			OwnershipPolicy::release(m_value);
		}

		void swap(unique_value &other)
		{
			auto left = std::tie(
						static_cast<OwnershipPolicy &>(*this),
						m_value);
			auto right = std::tie(
						static_cast<OwnershipPolicy &>(other),
						other.m_value);
			left.swap(right);
		}

		Value &operator * ()
		{
			return m_value;
		}

		Value const &operator * () const
		{
			return m_value;
		}

		Value *operator -> ()
		{
			return &m_value;
		}

		Value const *operator -> () const
		{
			return &m_value;
		}

		//TODO: ==, !=, <, .., std::hash

	private:

		Value m_value;
	};
}


#endif
