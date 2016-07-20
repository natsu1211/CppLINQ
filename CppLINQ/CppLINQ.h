#pragma once

#include <vector>
#include <string>
#include <list>
#include <deque>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <memory>

#include <utility>
#ifdef _MSC_VER
#include <xutility>
#else
#define __thiscall
#endif

namespace LL
{
	namespace iterators
	{
		template<typename TIterator, typename TFunction>
		class where_iterator
		{
			typedef where_iterator<TIterator, TFunction> TSelf;
		private:
			TIterator current_;
			TIterator end_;
			TFunction func_;
			
		public:
			where_iterator() = default;
			where_iterator(TIterator current, TIterator end, TFunction func)
				:current_(current), end_(end), func_(func)
			{
				while (current_ != end_ && !func_(*current_))
				{
					++current_;
				}
			}

			TSelf& operator++()
			{
				while (current_ != end_)
				{
					++current_;
					if (current_ == end_) break;
					if (func_(*current_)) break;
				}
				return *this;
			}

			TSelf operator++(int)
			{
				TSelf self = *this;
				while (current_ != end_)
				{
					++current_;
					if (current_ == end_) break;
					if (func_(*current_)) break;
				}
				return self;
			}

			auto operator*() const
			{
				return *current_;
			}

			bool operator==(const TSelf& iter) const
			{
				return current_ == iter.current_;
			}

			bool operator!=(const TSelf& iter) const
			{
				return current_ != iter.current_;
			}
		};

		template<typename TIterator>
		class skip_iterator
		{
			typedef skip_iterator<TIterator> TSelf;
		private:
			TIterator current_;
			TIterator end_;

		public:
			skip_iterator() = default;
			skip_iterator(TIterator current, TIterator end, int skip_count)
				:current_(current), end_(end)
			{
				while (skip_count > 0 && current_ != end_)
				{
					--skip_count;
					++current_;
				}
			}

			TSelf& operator++()
			{
				++current_;
				return *this;
			}

			TSelf operator++(int)
			{
				TSelf self = *this;
				++current_;
				return self;
			}

			auto operator*() const
			{
				return *current_;
			}

			bool operator==(const TSelf& iter) const
			{
				return current_ == iter.current_;
			}

			bool operator!=(const TSelf& iter) const
			{
				return current_ != iter.current_;
			}
		};
	}

	namespace iterators
	{
		template<typename TIterator, typename TFunction>
		using where_iter = where_iterator<TIterator, TFunction>;

		template<typename TIterator>
		using skip_iter = skip_iterator<TIterator>;
	}

	template<typename TIterator>
	class Queryable;

	template <typename TIterator>
	constexpr Queryable<TIterator> from(const TIterator &begin, const TIterator &end)
	{
		return Queryable<TIterator>(begin, end);
	}

	template<typename TContainer>
	constexpr auto from(const TContainer &container)
	{
		return Queryable<decltype(std::begin(container))>(std::begin(container), std::end(container));
	}

	template<typename TIterator>
	class Queryable
	{
	private:
		TIterator begin_;
		TIterator end_;
	public:
		constexpr Queryable(){}
		constexpr Queryable(const TIterator &begin, const TIterator &end)
			:begin_(begin), end_(end){}

		TIterator begin() const
		{
			return begin_;
		}

		TIterator end() const
		{
			return end_;
		}
		//where
		template<typename TFunction>
		Queryable<iterators::where_iter<TIterator, TFunction>> where(const TFunction& func) const
		{
			return Queryable<iterators::where_iter<TIterator, TFunction>>(
				iterators::where_iter<TIterator, TFunction>(begin_, end_, func),
				iterators::where_iter<TIterator, TFunction>(end_, end_, func)
				);
		}
		//skip
		Queryable<iterators::skip_iter<TIterator>> skip(int count) const
		{
			return Queryable<iterators::skip_iter<TIterator>>(
				iterators::skip_iter<TIterator>(begin_, end_, count),
				iterators::skip_iter<TIterator>(end_, end_, count)
				);
		}

	};
}

