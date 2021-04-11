// Original file license:
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt in the repository of the original
// 
// https://raw.githubusercontent.com/lewissbaker/cppcoro/master/include/cppcoro/generator.hpp
#pragma once

#include <coroutine>
#include <exception>

namespace CheaPU
{
	template<typename T>
	class StepByStep;

	namespace detail
	{
		/** Holds the value (or exception), but what really matters is that you can suspend execution where you
		   want (thanks to yield_value, that allows to use co_yield in the coroutine body) or terminate it 
		   (thanks to return_value/co_return). */
		template<typename T>
		class StepByStep_promise
		{
		public:
			using value_type = std::remove_reference_t<T>;

			StepByStep_promise() = default;

			StepByStep<T> get_return_object() noexcept;

			constexpr std::suspend_always initial_suspend() const noexcept { return {}; }
			constexpr std::suspend_always final_suspend() const noexcept { return {}; }

			std::suspend_always yield_value(std::remove_reference_t<T>&& value) noexcept
			{
				m_value = value;
				return {};
			}

			void return_value(T value) noexcept
			{
				m_value = value;
			}

			void unhandled_exception()
			{
				m_exception = std::current_exception();
			}

			value_type value() const noexcept
			{
				return m_value;
			}

			// Don't allow any use of 'co_await' inside the StepByStep coroutine.
			template<typename U>
			std::suspend_never await_transform(U&& value) = delete;

			void rethrow_if_exception()
			{
				if (m_exception)
				{
					std::rethrow_exception(m_exception);
				}
			}

		private:
			value_type m_value;
			std::exception_ptr m_exception;
		};

		
	}

	template<typename T>
	class [[nodiscard]] StepByStep
	{
	public:

		using promise_type = detail::StepByStep_promise<T>;
		
		/** Basic coroutine that you can call multiple times to execute whatever code is in it
		    one step at a time. 
			
			This was copy-pasted from the CppCoro generator coroutine (https://github.com/lewissbaker/cppcoro).

			I removed the iterator interface because it makes no sense to run CPU istructions in a for-each cycle
			and moved the value-access code of the dereference operator in other parts. I left everything else
			"as-is". There may be "leftovers" that can be removed.

			I simplified the value access (no more provisions for references, for example), because I don't plan
			to use the value at all. At the same time, I could not figure out how to implement a "yield/return void"
			coroutine (see also https://stackoverflow.com/questions/51680974/can-co-yield-return-nothing).

			You can call the operator() to start the coroutine/instruction, which then can stop with
			co_yield (or co_return at the last stop). You can query the StepByStep object to know if it is completed.

			I think T can be any basic type. I did not test pointers, objects... because I don't need them.

			This may not be the best method do implement the CPU "microcode" (see for example https://www.youtube.com/watch?v=qJgsuQoy9bc).
			But I want to keep the clock outside the CPU, call out each cycle, as if it was the "tick" method in a game
			engine framework - so I must "freeze" the execution between cycles.

			It was either this, either iterating on vector of lambdas. I wonder if there is a simpler way.
		*/
		StepByStep() noexcept
			: m_coroutine(nullptr)
		{}

		StepByStep(StepByStep&& other) noexcept
			: m_coroutine(other.m_coroutine)
		{
			other.m_coroutine = nullptr;
		}

		StepByStep(const StepByStep& other) = delete;

		~StepByStep()
		{
			if (m_coroutine)
			{
				m_coroutine.destroy();
			}
		}

		StepByStep& operator=(StepByStep other) noexcept
		{
			swap(other);
			return *this;
		}

		void swap(StepByStep& other) noexcept
		{
			std::swap(m_coroutine, other.m_coroutine);
		}

		T operator()() noexcept
		{
			if (m_coroutine)
			{
				m_coroutine.resume();
				if (m_coroutine.done())
				{
					m_coroutine.promise().rethrow_if_exception();  // And crash badly, as this should be noexcept... but should it?
				}
			}
			return m_coroutine.promise().value();  // But... it should be in the "if"...
		}

		/** Tells you if there are more steps to do (that is, the code did
		not run to the end, there was no call to co_return yet). */
		bool completed() noexcept
		{
			return m_coroutine.done();
		}

	private:

		friend class detail::StepByStep_promise<T>;

		explicit StepByStep(std::coroutine_handle<promise_type> coroutine) noexcept
			: m_coroutine(coroutine)
		{}

		std::coroutine_handle<promise_type> m_coroutine;

	};

	template<typename T>
	void swap(StepByStep<T>& a, StepByStep<T>& b)
	{
		a.swap(b);
	}

	namespace detail
	{
		template<typename T>
		StepByStep<T> StepByStep_promise<T>::get_return_object() noexcept
		{
			using coroutine_handle = std::coroutine_handle<StepByStep_promise<T>>;
			return StepByStep<T>{ coroutine_handle::from_promise(*this) };
		}
	}

	template<typename FUNC, typename T>
	StepByStep<std::invoke_result_t<FUNC&, typename StepByStep<T>::iterator::reference>> fmap(FUNC func, StepByStep<T> source)
	{
		for (auto&& value : source)
		{
			co_yield std::invoke(func, static_cast<decltype(value)>(value));
		}
	}
}

