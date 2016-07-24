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
		//filter, mutate
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


		template<typename TIterator, typename TFunction>
		class select_iterator
		{
			typedef select_iterator<TIterator, TFunction> TSelf;
		private:
			TIterator current_;
			TFunction func_;
		public:
			select_iterator() = default;
			select_iterator(TIterator current, TFunction func)
				:current_(current), func_(func)
			{

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
				return func_(*current_);
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



		//skip, take
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
				if (current_ != end_)
				{
					++current_;
				}
				return *this;
			}

			TSelf operator++(int)
			{
				TSelf self = *this;
				if (current_ != end_)
				{
					++current_;
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

		template<typename TIterator, typename TFunction>
		class skip_while_iterator
		{
			typedef skip_while_iterator<TIterator, TFunction> TSelf;
		private:
			TIterator current_;
			TIterator end_;
			TFunction func_;

		public:
			skip_while_iterator() = default;
			skip_while_iterator(TIterator current, TIterator end, TFunction func)
				:current_(current), end_(end), func_(func)
			{
				while (current_ != end_ && func_(*current_) )
				{
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

		template<typename TIterator>
		class take_iterator
		{
			typedef take_iterator<TIterator> TSelf;
		private:
			TIterator current_;
			TIterator end_;
			int count_;
			int cur_count_;
		public:
			take_iterator() = default;
			take_iterator(TIterator current, TIterator end, int take_count)
				:current_(current), end_(end), count_(take_count), cur_count_(0)
			{
				if (cur_count_ == count_)
				{
					current_ = end_;
				}
			}

			TSelf& operator++()
			{
				if(++cur_count_ == count_)
				{
					current_ = end_;
				}
				else if(current_ != end_)
				{
					++current_;
				}
				return *this;
			}

			TSelf operator++(int)
			{
				TSelf self = *this;
				if (++cur_count_ == count_)
				{
					current_ = end_;
				}
				else if(current_ != end)
				{
					++current_;
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

		template<typename TIterator, typename TFunction>
		class take_while_iterator
		{
			typedef take_while_iterator<TIterator, TFunction> TSelf;
		private:
			TIterator current_;
			TIterator end_;
			TFunction func_;

		public:
			take_while_iterator() = default;
			take_while_iterator(TIterator current, TIterator end, TFunction func)
				:current_(current), end_(end), func_(func)
			{
				if (current_ != end_ && !func_(*current_))
				{
					current_ = end_;
				}
			}

			TSelf& operator++()
			{
				if (current_ != end_ && ++current_ != end_)
				{
					if (!(func_(*current_)))
					{
						current_ = end_;
					}
				}
				return *this;
			}

			TSelf operator++(int)
			{
				TSelf self = *this;
				if (current_ != end_ && ++current_ != end_)
				{
					if (!(func_(*current_)))
					{
						current_ = end_;
					}
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

	}

	namespace iterators
	{
		template<typename TIterator, typename TFunction>
		using where_iter = where_iterator<TIterator, TFunction>;

		template<typename TIterator>
		using skip_iter = skip_iterator<TIterator>;

		template<typename TIterator, typename TFunction>
		using skip_while_iter = skip_while_iterator<TIterator, TFunction>;

		template<typename TIterator>
		using take_iter = take_iterator<TIterator>;

		template<typename TIterator, typename TFunction>
		using take_while_iter = take_while_iterator<TIterator, TFunction>;
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
		//skip_while
		template<typename TFunction>
		Queryable<iterators::skip_while_iter<TIterator, TFunction>> skip_while(const TFunction& func) const
		{
			return Queryable<iterators::skip_while_iter<TIterator, TFunction>>(
				iterators::skip_while_iter<TIterator, TFunction>(begin_, end_, func),
				iterators::skip_while_iter<TIterator, TFunction>(end_, end_, func)
				);
		}
		//take
		Queryable<iterators::take_iter<TIterator>> take(int count) const
		{
			return Queryable<iterators::take_iter<TIterator>>(
				iterators::take_iter<TIterator>(begin_, end_, count),
				iterators::take_iter<TIterator>(end_, end_, count)
				);
		}
		//take_while
		template<typename TFunction>
		Queryable<iterators::take_while_iter<TIterator, TFunction>> take_while(const TFunction& func) const
		{
			return Queryable<iterators::take_while_iter<TIterator, TFunction>>(
				iterators::take_while_iter<TIterator, TFunction>(begin_, end_, func),
				iterators::take_while_iter<TIterator, TFunction>(end_, end_, func)
				);
		}
	};
}

