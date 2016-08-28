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
#include <array>
#include <utility>
#include <exception>

namespace LL
{
	//excption
	class linq_exception 
	{
	private:
		std::string message_;
	public:
		linq_exception() = default;

		linq_exception(const std::string& message)
		{
			message_ = message;
		}

		virtual const std::string& what() const noexcept
		{
			return message_;
		}
	};

	template<typename TIterator>
	using clean_type = typename std::remove_const<typename std::remove_reference<TIterator>::type>::type;


	template<typename TIterator>
	using value_type = decltype(**(TIterator*)0);

	template<typename T, bool B = false>
	struct default_type
	{
		typedef decltype(nullptr) type;
	};

	template<typename T>
	struct default_type<T, true>
	{
		typedef typename T type;
	};

	namespace iterators
	{
		//filter, mutate
		template<typename TIterator, typename TPredict>
		class where_iterator
		{
			typedef where_iterator<TIterator, TPredict> TSelf;
		private:
			TIterator current_;
			TIterator end_;
			TPredict func_;

		public:
			where_iterator() = default;
			where_iterator(const TIterator& current, const TIterator& end, const TPredict& func)
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

			const TSelf operator++(int)
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

			auto operator*() const -> decltype(*current_)
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


		template<typename TIterator, typename TPredict>
		class select_iterator
		{
			typedef select_iterator<TIterator, TPredict> TSelf;
		private:
			TIterator current_;
			TIterator end_;
			TPredict func_;
		public:
			select_iterator() = default;
			select_iterator(const TIterator& current, const TIterator& end, const TPredict& func)
				:current_(current), end_(end), func_(func)
			{

			}

			TSelf& operator++()
			{
				++current_;
				return *this;
			}

			const TSelf operator++(int)
			{
				TSelf self = *this;
				++current_;
				return self;
			}

			auto operator*() const -> decltype(func_(*current_))
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
		//select many
		template<typename TIterator, typename TPredict>
		class select_many_iterator
		{
		private:
			static TPredict get_function();
			static value_type<TIterator> get_value();
			using TSelf = select_many_iterator<TIterator, TPredict>;
			using TInner = clean_type<decltype(get_function()(get_value()))>;
			static TInner get_inner();
			using TInnerIterator = decltype(std::begin(get_inner()));
		private:
			TIterator current_;
			TIterator end_;
			TInnerIterator inner_current_;
			TInnerIterator inner_end_;
			TPredict func_;

		public:
			select_many_iterator() = default;
			select_many_iterator(const TIterator& current, const TIterator& end, const TPredict& func)
				:current_(current), end_(end), func_(func)
			{
				if (current_ != end_)
				{
					auto cur = func_(*current_);
					inner_current_ = std::begin(cur);
					inner_end_ = std::end(cur);
				}

			}

			TSelf& operator++()
			{
				if(++inner_current_ == inner_end_ && current_ != end_)
				{
					if (++current_ != end_)
					{
						auto cur = func_(*current_);
						inner_current_ = std::begin(cur);
						inner_end_ = std::end(cur);
					}
				}
				return *this;
			}

			const TSelf operator++(int)
			{
				TSelf self = *this;
				if(++inner_current_ == inner_end_ && current_ != end_)
				{
					if (++current_ != end_)
					{
						auto cur = func_(*current_);
						inner_current_ = std::begin(cur);
						inner_end_ = std::end(cur);
					}
				}
				return self;
			}

			auto operator*() const -> decltype(*inner_current_)
			{
				return *inner_current_;
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
		//single with parameter
		template<typename TIterator, typename TPredict>
		class single_iterator
		{
			typedef single_iterator<TIterator, TPredict> TSelf;
		private:
			TIterator current_;
			TIterator end_;
			TPredict func_;
			TIterator single_;
		public:
			single_iterator() = default;
			single_iterator(const TIterator& current, const TIterator& end, const TPredict& func)
				:current_(current), end_(end), func_(func), single_(current_)
			{
				for (auto it = current_; it != end_; ++it)
				{
					if (func_(*it))
					{
						single_ = it;
					}
				}
			}

			TSelf& operator++()
			{
				current_ = end_;
				return *this;
			}

			const TSelf operator++(int)
			{
				TSelf self = *this;
				current_ = end_;
				return self;
			}

			auto operator*() const -> decltype(*single_)
			{
				return *single_;
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
			skip_iterator(const TIterator& current, const TIterator& end, int skip_count)
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

			const TSelf operator++(int)
			{
				TSelf self = *this;
				++current_;
				return self;
			}

			auto operator*() const -> decltype(*current_)
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

		template<typename TIterator, typename TPredict>
		class skip_while_iterator
		{
			typedef skip_while_iterator<TIterator, TPredict> TSelf;
		private:
			TIterator current_;
			TIterator end_;
			TPredict func_;

		public:
			skip_while_iterator() = default;
			skip_while_iterator(const TIterator& current, const TIterator& end, const TPredict& func)
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

			const TSelf operator++(int)
			{
				TSelf self = *this;
				++current_;
				return self;
			}

			auto operator*() const -> decltype(*current_)
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
			take_iterator(const TIterator& current, const TIterator& end, int take_count)
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

			const TSelf operator++(int)
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

			auto operator*() const -> decltype(*current_)
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

		template<typename TIterator, typename TPredict>
		class take_while_iterator
		{
			typedef take_while_iterator<TIterator, TPredict> TSelf;
		private:
			TIterator current_;
			TIterator end_;
			TPredict func_;

		public:
			take_while_iterator() = default;
			take_while_iterator(const TIterator& current, const TIterator& end, const TPredict& func)
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

			const TSelf operator++(int)
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

			auto operator*() const -> decltype(*current_)
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

		template<typename TIterator, typename TResult>
		class cast_iterator
		{
			typedef cast_iterator<TIterator, TResult> TSelf;
		private:
			TIterator current_;
			TIterator end_;

		public:
			cast_iterator() = default;
			cast_iterator(const TIterator& current, const TIterator& end)
				:current_(current), end_(end)
			{
			}

			TSelf& operator++()
			{
				++current_;
				return *this;
			}

			const TSelf operator++(int)
			{
				TSelf self = *this;
				++current_;
				return self;
			}

			TResult operator*() const
			{
				return (TResult)(*current_);
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

		template<typename TIterator1, typename TIterator2>
		class concat_iterator
		{
			typedef concat_iterator<TIterator1, TIterator2> TSelf;
		private:
			TIterator1 current1_;
			TIterator1 end1_;
			TIterator2 current2_;
			TIterator2 end2_;
		public:
			concat_iterator() = default;
			concat_iterator(const TIterator1& current1, const TIterator1& end1, const TIterator2& current2, const TIterator2& end2)
				:current1_(current1), end1_(end1), current2_(current2), end2_(end2)
			{

			}

			TSelf& operator++()
			{
				if (current1_ != end1_)
				{
					++current1_;
				}
				else if (current2_ != end2_)
				{
					++current2_;
				}

				return *this;
			}

			const TSelf operator++(int)
			{
				TSelf self = *this;
				if (current1_ != end1_)
				{
					++current_1;
				}
				else if (current2_ != end2_)
				{
					++current2_;
				}
				return self;
			}

			auto operator*() const -> decltype(*current1_)
			{
				return (current1_ != end1_) ? *current1_ : *current2_;
			}

			bool operator==(const TSelf& iter) const
			{
				if (current1_ != end1_) return current1_ == iter.current1_;
				return current2_ == iter.current2_;
			}

			bool operator!=(const TSelf& iter) const
			{
				if (current1_ != end1_) return current1_ != iter.current1_;
				return current2_ != iter.current2_;
			}
		};

		template<typename TValue>
		class value_iterator
		{
			typedef value_iterator<TValue> TSelf;
		private:
			std::shared_ptr<std::vector<TValue>> values_;
			typename std::vector<TValue>::iterator current_;
			typename std::vector<TValue>::iterator end_;

		public:
			value_iterator() = default;
			value_iterator(const std::shared_ptr<std::vector<TValue>>& values)
			:values_(values), current_(values->begin()), end_(values->end())
			{

			}

			TSelf& operator++()
			{
				++current_;
				return *this;
			}

			const TSelf operator++(int)
			{
				TSelf self = *this;
				++current_;
				return self;
			}

			auto operator*() const -> decltype(*current_)
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

		template<typename TContainerPointer>
		class copy_iterator
		{
			typedef copy_iterator<TContainerPointer> TSelf;
		private:
			TContainerPointer p_;
			using TContainerIterator = decltype(p_->begin());
			TContainerIterator current_;
			TContainerIterator end_;

		public:
			copy_iterator() = default;
			copy_iterator(const TContainerPointer& p, const TContainerIterator& current, const TContainerIterator& end)
				:p_(p), current_(current), end_(end)
			{

			}

			TSelf& operator++()
			{
				++current_;
				return *this;
			}

			const TSelf operator++(int)
			{
				TSelf self = *this;
				++current_;
				return self;
			}

			auto operator*() const -> decltype(*current_)
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
		template<typename TIterator, typename TPredict>
		using where_iter = where_iterator<TIterator, TPredict>;

		template<typename TIterator, typename TPredict>
		using select_iter = select_iterator<TIterator, TPredict>;

		template<typename TIterator, typename TPredict>
		using select_many_iter = select_many_iterator<TIterator, TPredict>;

		template<typename TIterator, typename TPredict>
		using single_iter = single_iterator<TIterator, TPredict>;

		template<typename TIterator>
		using skip_iter = skip_iterator<TIterator>;

		template<typename TIterator, typename TPredict>
		using skip_while_iter = skip_while_iterator<TIterator, TPredict>;

		template<typename TIterator>
		using take_iter = take_iterator<TIterator>;

		template<typename TIterator, typename TPredict>
		using take_while_iter = take_while_iterator<TIterator, TPredict>;

		template<typename TIterator, typename TResult>
		using cast_iter = cast_iterator<TIterator, TResult>;

		template<typename TIterator1, typename TIterator2>
		using concat_iter = concat_iterator<TIterator1, TIterator2>;

		template<typename TValue>
		using value_iter = value_iterator<TValue>;

		template<typename TContainerPointer>
		using copy_iter = copy_iterator<TContainerPointer>;
	}

	template<typename TIterator>
	class Queryable;

	template<typename T>
	class Queryables;

	template <typename TIterator>
	constexpr Queryable<TIterator> from(const TIterator& begin, const TIterator& end)
	{
		return Queryable<TIterator>(begin, end);
	}

	template<typename TContainer>
	constexpr auto from(const TContainer &container) -> decltype(Queryable<decltype(std::begin(container))>(std::begin(container), std::end(container)))
	{
		return Queryable<decltype(std::begin(container))>(std::begin(container), std::end(container));
	}

	template<typename TValue>
	constexpr Queryable<iterators::value_iter<TValue>> from_value(const TValue& value)
	{
		auto p = std::make_shared<std::vector<TValue>>();
		p->push_back(value);
		return Queryable<iterators::value_iter<TValue>>(
			iterators::value_iter<TValue>(p),
			iterators::value_iter<TValue>(p));
	}


	template<typename TIterator>
	class Queryable
	{
		using TElement = clean_type<value_type<TIterator>>;
	private:
		TIterator begin_;
		TIterator end_;
	public:
		constexpr Queryable() = default;
		constexpr Queryable(const TIterator& begin, const TIterator& end)
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
		template<typename TPredict>
		Queryable<iterators::where_iter<TIterator, TPredict>> where(const TPredict& func) const
		{
			return Queryable<iterators::where_iter<TIterator, TPredict>>(
				iterators::where_iter<TIterator, TPredict>(begin_, end_, func),
				iterators::where_iter<TIterator, TPredict>(end_, end_, func)
				);
		}
		//select
		template<typename TPredict>
		Queryable<iterators::select_iter<TIterator, TPredict>> select(const TPredict& func) const
		{
			return Queryable<iterators::select_iter<TIterator, TPredict>>(
				iterators::select_iter<TIterator, TPredict>(begin_, end_, func),
				iterators::select_iter<TIterator, TPredict>(end_, end_, func)
				);
		}
		//select many
		template<typename TPredict>
		Queryable<iterators::select_many_iter<TIterator, TPredict>> select_many(const TPredict& func) const
		{
			return Queryable<iterators::select_many_iter<TIterator, TPredict>>(
				iterators::select_many_iter<TIterator, TPredict>(begin_, end_, func),
				iterators::select_many_iter<TIterator, TPredict>(end_, end_, func)
				);
		}
		//single without parameter
		TElement single() const
		{
			auto it = begin_;
			if (++it != end_) throw linq_exception("The collection should have only one value.");
			return *begin_;
		}
		//single with parameter
		template<typename TPredict>
		TElement single(const TPredict& func) const
		{
			if (empty()) throw linq_exception("Empty collection.");
			int cnt = 0;
			for (auto it = begin_; it != end_; ++it)
			{
				if (func(*it))
				{
					++cnt;
				}
			}
			if (cnt == 0) throw linq_exception("No value found");
			else if (cnt != 1) throw linq_exception("More than one value found");
			auto queryable = Queryable<iterators::single_iter<TIterator, TPredict>>(
				iterators::single_iter<TIterator, TPredict>(begin_, end_, func),
				iterators::single_iter<TIterator, TPredict>(end_, end_, func)
				);
			return *queryable;
		}
		//single_or_default without parameter
		TElement single_or_default() const
		{
			auto it = begin_;
			//empty, require TElement has a default constructor, TODO
			if (it == end_) return TElement{};

			if (++it != end_) throw linq_exception("The collection should have only one value.");
			return *begin_;
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
		template<typename TPredict>
		Queryable<iterators::skip_while_iter<TIterator, TPredict>> skip_while(const TPredict& func) const
		{
			return Queryable<iterators::skip_while_iter<TIterator, TPredict>>(
				iterators::skip_while_iter<TIterator, TPredict>(begin_, end_, func),
				iterators::skip_while_iter<TIterator, TPredict>(end_, end_, func)
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
		template<typename TPredict>
		Queryable<iterators::take_while_iter<TIterator, TPredict>> take_while(const TPredict& func) const
		{
			return Queryable<iterators::take_while_iter<TIterator, TPredict>>(
				iterators::take_while_iter<TIterator, TPredict>(begin_, end_, func),
				iterators::take_while_iter<TIterator, TPredict>(end_, end_, func)
				);
		}
		//aggregate
		template<typename TPredict>
		TElement aggregate(const TPredict& func) const
		{
			if(empty()) throw linq_exception("Empty Collection");
			TElement result = 0;
			for(auto iter = begin_; iter != end_; ++iter)
			{
				result = func(*iter, result);
			}
			return result;
		}
		//average with function
		template<typename TPredict>
		TElement average(const TPredict& func) const
		{
			if(empty()) throw linq_exception("Empty Collection");
			TElement sum = 0;
			int cnt = 0;
			for(auto iter = begin_; iter != end_; ++iter)
			{
				++cnt;
				sum += func(*iter);
			}
			return sum/cnt;
		}
		//average
		TElement average() const
		{
			return average([](TElement e){return e;});
		}
		//max
		TElement max() const
		{
			return aggregate([](TElement a, TElement b){return a>b?a:b;});
		}
		//min
		TElement min() const
		{
			return aggregate([](TElement a, TElement b){return a<b?a:b;});
		}
		//sum
		TElement sum() const
		{
			return aggregate([](TElement a, TElement b){return b+a;});
		}
		//any
		template<typename TPredict>
		bool any(const TPredict& func) const
		{
			for(auto iter = begin_; iter != end_; ++iter)
			{
				if(func(*iter))
				{
					return true;
				}
			}
			return false;
		}
		//all
		template<typename TPredict>
		bool all(const TPredict& func) const
		{
			for(auto iter = begin_; iter != end_; ++iter)
			{
				if(!func(*iter))
				{
					return false;
				}
			}
			return true;
		}
		//count
		int count() const
		{
			int cnt = 0;
			for(auto iter = begin_; iter != end_; ++iter)
			{
				++cnt;
			}
			return cnt;
		}
		//long count
		long long_count() const
		{
			long cnt = 0;
			for (auto iter = begin_; iter != end_; ++iter)
			{
				++cnt;
			}
			return cnt;
		}
		//sequence equal
		template<typename TIterator2>
		bool sequence_equal(const Queryable<TIterator2> &seq) const
		{
			auto end1 = this->end_;
			auto end2 = seq.end_;
			for(auto it1 = this->begin, it2 = seq.begin_; it1 != end1 && it2 != end2; ++it1, ++it2)
			{
				if(*it1 != *it2) return false;
			}
			return it1 == end1 && it2 == end2;
		}
		//to vector
		std::vector<TElement> to_vector() const
		{
			std::vector<TElement> vector;
			for(auto iter = begin_; iter != end_; ++iter)
			{
				vector.push_back(*iter);
			}
			return std::move(vector);
		}
		//to list
		std::list<TElement> to_list() const
		{
			std::list<TElement> list;
			for (auto iter = begin_; iter != end_ ; ++iter)
			{
				list.push_back(*iter);
			}
			return std::move(list);
		}
		//to set
		std::set<TElement> to_set() const
		{
			std::set<TElement> set;
			for (auto iter = begin_; iter != end_ ; ++iter)
			{
				set.insert(*iter);
			}
			return std::move(set);
		}
		//to unordered_set
		std::set<TElement> to_unordered_set() const
		{
			std::unordered_set<TElement> set;
			for (auto iter = begin_; iter != end_ ; ++iter)
			{
				set.insert(*iter);
			}
			return std::move(set);
		}
		//to map
		template<typename TPredict1, typename TPredict2>
		auto to_map(const TPredict1& keySelector, const TPredict2& valueSelector) const -> decltype(std::map<decltype(keySelector(*(TElement*)0)), decltype(vaulueSelector(*(TElement*)0))>())
		{
			std::map<decltype(keySelector(*(TElement*)0)), decltype(vaulueSelector(*(TElement*)0))> map;
			for (auto iter = begin_; iter != end_ ; ++iter)
			{
				map.insert(std::make_pair<keySelector(*iter),valueSelector(*iter)>);
			}
			return std::move(map);
		}
		//to map
		template<typename TPredict>
		auto to_map(const TPredict& keySelector) const -> decltype(std::map<decltype(keySelector(*(TElement*)0)), TElement>())
		{
			std::map<decltype(keySelector(*(TElement*)0)), TElement> map;
			for (auto iter = begin_; iter != end_ ; ++iter)
			{
				map.insert(std::make_pair<keySelector(*iter),*iter>);
			}
			return std::move(map);
		}

		//to unordered_map
		template<typename TPredict1, typename TPredict2>
		auto to_unordered_map(const TPredict1& keySelector, const TPredict2& valueSelector) const -> decltype(std::unordered_map<decltype(keySelector(*(TElement*)0)), decltype(vaulueSelector(*(TElement*)0))>())
		{
			std::unordered_map<decltype(keySelector(*(TElement*)0)), decltype(vaulueSelector(*(TElement*)0))> map;
			for (auto iter = begin_; iter != end_ ; ++iter)
			{
				map.insert(std::make_pair<keySelector(*iter),valueSelector(*iter)>);
			}
			return std::move(map);
		}

		//to unordered_map
		template<typename TPredict>
		auto to_unordered_map(const TPredict& keySelector) const -> decltype(std::unordered_map<decltype(keySelector(*(TElement*)0)), TElement>())
		{
			std::unordered_map<decltype(keySelector(*(TElement*)0)), TElement> map;
			for (auto iter = begin_; iter != end_ ; ++iter)
			{
				map.insert(std::make_pair<keySelector(*iter),*iter>);
			}
			return std::move(map);
		}

		//cast
		template<typename TResult>
		Queryable<iterators::cast_iter<TIterator, TResult>> cast() const
		{
			return Queryable<iterators::cast_iter<TIterator, TResult>>(
				iterators::cast_iter<TIterator, TResult>(begin_, end_),
				iterators::cast_iter<TIterator, TResult>(end_, end_)
				);
		}

		//concat
		template<typename TIterator2>
		Queryable<iterators::concat_iter<TIterator, TIterator2>> concat(const Queryable<TIterator2>& iter2) const
		{
			return Queryable<iterators::concat_iter<TIterator, TIterator2>>(
				iterators::concat_iter<TIterator, TIterator2>(begin_, end_, iter2.begin(), iter2.end()),
				iterators::concat_iter<TIterator, TIterator2>(end_, end_, iter2.end(), iter2.end())
				);
		}
		//contains
		bool contains(const TElement& item) const
		{
			for (auto iter = begin_; iter != end_; ++iter)
			{
				if (*iter == item) return true;
			}
			return false;
		}
		//first without parameter
		TElement first() const
		{
			if (empty()) throw linq_exception("empty collection");
			return *begin_;
		}

		//first with parameter
		template<typename TPredict>
		TElement first(const TPredict& func) const
		{
			if (empty()) throw linq_exception("empty collection");
			auto iter = begin_;
			for (auto iter = begin_; iter != end_; ++iter)
			{
				if (func(*iter)) return *iter;
			}
			throw linq_exception("Not found");
		}

		//last without parameter
		TElement last() const
		{
			if (empty()) throw linq_exception("empty collection");
			TElement ret{};
			for (auto iter = begin_; iter != end_; ++iter)
			{
				ret = *iter;
			}
			return ret;
		}

		//last with parameter
		template<typename TPredict>
		TElement last(const TPredict& func) const
		{
			if (empty()) throw linq_exception("empty collection");
			TElement ret{};
			std::vector<TElement> v;
			for (auto iter = begin_; iter != end_; ++iter)
			{
				if (func(*iter)) v.pushback(*iter);
			}
			if(v.empty()) throw linq_exception("Not found");
			return v.back();
		}

		//first_or_default without parameter
		TElement first_or_default() const
		{
			if (empty()) return TElement{};
			return *begin_;
		}

		//first_or_default with parameter
		template<typename TPredict>
		TElement first_or_default(const TPredict& func) const
		{
			if (empty()) return TElement{};
			auto iter = begin_;
			for (auto iter = begin_; iter != end_; ++iter)
			{
				if (func(*iter)) return *iter;
			}
			return TElement{};
		}

		//last_or_default without parameter
		TElement last_or_default() const
		{
			TElement ret{};
			for (auto iter = begin_; iter != end_; ++iter)
			{
				ret = *iter;
			}
			return ret;
		}

		//last_or_default with parameter
		template<typename TPredict>
		TElement last_or_default(const TPredict& func) const
		{
			TElement ret{};
			for (auto iter = begin_; iter != end_; ++iter)
			{
				if (func(*iter)) ret = *iter;
			}
			return ret;
		}
		//empty
		inline bool empty() const
		{
			return begin_ == end_;
		}
		//default_if_empty without parameter
		Queryables<TElement> default_if_empty() const
		{
			if (empty())
			{
				return default_if_empty(TElement());
			}
			else
			{
				return *this;
			}
		}

		//default_if_empty with parameter
		Queryables<TElement> default_if_empty(const TElement& default_value) const
		{
			if (empty())
			{
				return from_value(default_value);
			}
			else
			{
				return *this;
			}
		}
		//element_at
		TElement element_at(int index) const
		{
			if(index >= 0)
			{
				int count = 0;
				for(auto iter = begin_; iter != end_; ++iter)
				{
					if(count == index) return *iter;
					++count;
				}
			}
			throw linq_exception("Index out of range");
		}
		//element_at_or_default
		TElement element_at_or_default(int index) const
		{
			if(index >= 0)
			{
				int count = 0;
				for(auto iter = begin_; iter != end_; ++iter)
				{
					if(count == index) return *iter;
					++count;
				}
			}
			return TElement{};
		}
		//distinct
		Queryable<iterators::copy_iter<std::shared_ptr<std::set<TElement>>>> distinct() const
		{
			if (empty()) throw linq_exception("Empty Collection");
			auto set = std::make_shared<std::set<TElement>>();
			for (auto iter = begin_; iter != end_; ++iter)
			{
				set->insert(*iter);
			}
			return Queryable<iterators::copy_iter<std::shared_ptr<std::set<TElement>>>>(
				iterators::copy_iter<std::shared_ptr<std::set<TElement>>>(set, set->begin(), set->end()),
				iterators::copy_iter<std::shared_ptr<std::set<TElement>>>(set, set->end(), set->end())
				);
		}
		//except
		template<typename TIterator2>
		Queryable<iterators::copy_iter<std::shared_ptr<std::vector<TElement>>>> except(const Queryable<TIterator2>& q) const
		{
			if(empty() || q.empty()) throw linq_exception("Empty Collection");
			auto v = std::make_shared<std::vector<TElement>>();
			std::set<TElement> set_ex(q.begin(), q.end());
			for (auto iter = begin_; iter != end_; ++iter)
			{
				//if element is not in exclude set
				if (set_ex.insert(*iter).second)
				{
					v->push_back(*iter);
				}
			}
			return Queryable<iterators::copy_iter<std::shared_ptr<std::vector<TElement>>>>(
				iterators::copy_iter<std::shared_ptr<std::vector<TElement>>>(v, v->begin(), v->end()),
				iterators::copy_iter<std::shared_ptr<std::vector<TElement>>>(v, v->end(), v->end())
				);

		}
		//intersect
		template<typename TIterator2>
		Queryable<iterators::copy_iter<std::shared_ptr<std::vector<TElement>>>> intersect(const Queryable<TIterator2>& q) const
		{
			if (empty() || q.empty()) throw linq_exception("Empty Collection");
			auto v = std::make_shared<std::vector<TElement>>();
			std::set<TElement> set(q.begin(), q.end());
			for (auto iter = begin_; iter != end_; ++iter)
			{
				if (!set.insert(*iter).second)
				{
					v->push_back(*iter);
				}
			}
			return Queryable<iterators::copy_iter<std::shared_ptr<std::vector<TElement>>>>(
				iterators::copy_iter<std::shared_ptr<std::vector<TElement>>>(v, v->begin(), v->end()),
				iterators::copy_iter<std::shared_ptr<std::vector<TElement>>>(v, v->end(), v->end())
				);
		}
		
		//union, use linq_union to avoid key word union
		template<typename TIterator2>
		Queryable<iterators::copy_iter<std::shared_ptr<std::set<TElement>>>> linq_union(const Queryable<TIterator2>& q) const
		{
			if (empty() || q.empty()) throw linq_exception("Empty Collection");
			return concat(q).distinct();
		}
		//join
		//zip
		//group_by
		//group_join

	};
}
