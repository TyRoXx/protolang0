#pragma once
#ifndef P0RUN_GARBAGE_COLLECTOR_HPP
#define P0RUN_GARBAGE_COLLECTOR_HPP


#include "p0common/final.hpp"
#include <stdexcept>


namespace p0
{
	namespace run
	{
		struct object;


		namespace sweep_mode
		{
			enum type
			{
				/** Ask the GC to destroy any number of unmarked objects. */
				partial,

				/** Ask the GC to destroy as many of the unmarked objects as possible. */
				full
			};
		}


		struct garbage_collector
		{
			virtual ~garbage_collector();

			/**
			 * @brief allocate Reserves a piece of memory which can hold any object of the given size.
			 * @param byte_size The size of the reserved memory in Bytes.
			 * @return Pointer to a piece of memory which must be freed with this->deallocate or committed as
			 *         a constructed object with this->commit_object to participate in garbage collection.
			 */
			virtual char *allocate(std::size_t byte_size) = 0;

			/**
			 * @brief deallocate Frees memory reserved by this->allocate which has not been constructed
			 *        as an object yet.
			 * @param storage Pointer to a piece of memory reserved by this->allocate.
			 *        Undefined behaviour if called with a nullptr.
			 */
			virtual void deallocate(char *storage) = 0;

			/**
			 * @brief commit_object Marks a piece of memory as a fully constructed object for garbage collection.
			 * @param constructed The callee is responsible for the object's destruction.
			 */
			virtual void commit_object(object &constructed) = 0;

			/**
			 * @brief unmark Prepares a garbage collection by unmarking all objects managed by this GC.
			 */
			virtual void unmark() = 0;

			/**
			 * @brief sweep Looks for objects which are not marked as being in use and destroys them.
			 *              You have to mark each and every object which is still required.
			 *              After sweep the behaviour is undefined if you access an object which you
			 *              had not marked as required before sweep.
			 * @param mode The GC may ignore the mode request and select a different strategy.
			 *             See sweep_mode::type for explanations for the modes.
			 */
			virtual void sweep(sweep_mode::type mode) = 0;
		};


		struct raw_storage PROTOLANG0_FINAL_CLASS
		{
			raw_storage();
			raw_storage(garbage_collector &gc, std::size_t byte_size);
			raw_storage(raw_storage &&other);
			~raw_storage();
			raw_storage &operator = (raw_storage &&other);
			void swap(raw_storage &other);

			template <class T, class ...Args>
			object &construct(Args && ...args)
			{
				object &constructed = *new (m_memory) T(std::forward<Args>(args)...);
				m_gc->commit_object(constructed);
				m_memory = nullptr;
				return constructed;
			}

		private:

			garbage_collector *m_gc;
			char *m_memory;
		};


		template <class T, class ...Args>
		object &construct_object(garbage_collector &gc, Args && ...args)
		{
			return raw_storage(gc, sizeof(T)).construct<T>(std::forward<Args>(args)...);
		}
	}
}


#endif
