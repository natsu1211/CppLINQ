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
#include <iostream>

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
			using TSelf = select_many_iterator<TIterator, TPredict>;
			TIterator current_;
			TIterator end_;
			TPredict func_;
			using TInner = decltype(func_(*current_));
			TInner cur_;
			size_t inner_index_;
		public:

			select_many_iterator() = default;
			select_many_iterator(const TIterator& current, const TIterator& end, const TPredict& func)
				:current_(current), end_(end), func_(func), inner_index_(0)
			{
				if (current_ != end_)
				{
					cur_ = func_(*current_);
				}
			}

			TSelf& operator++()
			{
				if(++inner_index_ >= std::distance(std::begin(cur_), std::end(cur_)))
				{
				 	if (++current_ != end_)
					{
						cur_ = func_(*current_);
					}
					inner_index_ = 0;
				}
				return *this;
			}

			const TSelf operator++(int)
			{
				TSelf self = *this;
                if(++inner_index_ >= std::distance(std::begin(cur_), std::end(cur_)))
				{
					if (++current_ != end_)
					{
						cur_ = func_(*current_);
					}
					inner_index_ = 0;
				}
				return self;
			}

			auto operator*() const -> decltype(*(std::begin(cur_) + inner_index_))
			{
				return *(std::begin(cur_) + inner_index_);
			}

			bool operator==(const TSelf& iter) const
			{
				return current_ == iter.current_ && inner_index_ == iter.inner_index_;
			}

			bool operator!=(const TSelf& iter) const
			{
				return current_ != iter.current_ || inner_index_ != iter.inner_index_;
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
				else if(current_ != end_)
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
					++current1_;
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
        template<typename TIterator1, typename TIterator2>
        class zip_iterator
        {
            typedef zip_iterator<TIterator1, TIterator2> TSelf;
        private:
            TIterator1 current1_;
            TIterator1 end1_;
            TIterator2 current2_;
            TIterator2 end2_;
			using TPair = std::pair<clean_type<decltype(*current1_)>, clean_type<decltype(*current2_)>>;
			std::vector<TPair> pairs_;
			size_t pair_index_;
        public:
            zip_iterator() = default;
            zip_iterator(const TIterator1& current1, const TIterator1& end1, const TIterator2& current2, const TIterator2& end2)
				:current1_(current1), end1_(end1), current2_(current2), end2_(end2), pair_index_(0)
            {
				auto c1 = current1;
				auto c2 = current2;
				while (c1 != end1_ && c2 != end2_)
				{
					pairs_.push_back(std::make_pair(*c1, *c2));
                    ++c1;
                    ++c2;
				}
				if (c1 != end1_ || c2 != end2_) throw linq_exception("The size of two sequence is not matched.");
            }
			TSelf& operator++()
			{
				++pair_index_;
				++current1_;
				++current2_;
				return *this;
			}

			const TSelf operator++(int)
			{
				TSelf self = *this;
				++pair_index_;
				++current1_;
				++current2_;
				return self;
			}

			TPair operator*() const
			{
				return pairs_[pair_index_];
			}

			bool operator==(const TSelf& iter) const
			{
				return current1_ == iter.current1_;
			}

			bool operator!=(const TSelf& iter) const
			{
				return current1_ != iter.current1_;
			}

		};

		template<typename TContainerPointer>
		class adapter_iterator
		{
			typedef adapter_iterator<TContainerPointer> TSelf;
		private:
			TContainerPointer p_;
			using TContainerIterator = decltype(p_->begin());
			TContainerIterator current_;
			TContainerIterator end_;

		public:
			adapter_iterator() = default;
			adapter_iterator(const TContainerPointer& p, const TContainerIterator& current, const TContainerIterator& end)
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

		template<typename TIterator1, typename TIterator2>
		using concat_iter = concat_iterator<TIterator1, TIterator2>;

		template<typename TContainerPointer>
		using adapter_iter = adapter_iterator<TContainerPointer>;

		template<typename TIterator1, typename TIterator2>
		using zip_iter = zip_iterator<TIterator1, TIterator2>;
	}

	template<typename TIterator>
	class Queryable;

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

    template<typename T>
    constexpr auto from(const std::initializer_list<T> &list) -> decltype(Queryable<decltype(std::begin(list))>(std::begin(list), std::end(list)))
    {
        return Queryable<decltype(std::begin(list))>(std::begin(list), std::end(list));
    }

	template<typename TIterator>
	class Queryable
	{
		using TSelf = Queryable<TIterator>;
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
		template<typename TInit, typename TPredict>
		TElement aggregate(const TInit &init, const TPredict& func) const
		{
			if(empty()) throw linq_exception("Empty Collection");
			auto result = init;
			for(auto iter = begin_; iter != end_; ++iter)
			{
				result = func(*iter, result);
			}
			return result;
		}
        template<typename TPredict>
        TElement aggregate(const TPredict& func) const
        {
            if(empty()) throw linq_exception("Empty Collection");
            auto iter = begin_;
            auto result = *iter;
            while(++iter != end_)
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
		template<typename TList>
		bool sequence_equal(const TList &seq) const
		{
			auto end1 = this->end_;
			auto end2 = std::end(seq);
            auto it1 = this->begin_;
            auto it2 = std::begin(seq);
			for (;it1 != end1 && it2 != end2; ++it1, ++it2)
			{
				if(*it1 != *it2) return false;
			}
			return it1 == end1 && it2 == end2;
		}

        template<typename T>
        bool sequence_equal(const std::initializer_list<T> &il) const
        {
            auto end1 = this->end();
            auto end2 = std::end(il);
            auto it1 = this->begin();
            auto it2 = std::begin(il);
            for (;it1 != end1 && it2 != end2; ++it1, ++it2)
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
				vector.emplace_back(*iter);
			}
			return vector;
		}
		//to list
		std::list<TElement> to_list() const
		{
			std::list<TElement> list;
			for (auto iter = begin_; iter != end_ ; ++iter)
			{
				list.emplace_back(*iter);
			}
			return list;
		}
		//to set
		std::set<TElement> to_set() const
		{
			std::set<TElement> set;
			for (auto iter = begin_; iter != end_ ; ++iter)
			{
				set.insert(*iter);
			}
			return set;
		}
		//to unordered_set
		std::set<TElement> to_unordered_set() const
		{
			std::unordered_set<TElement> set;
			for (auto iter = begin_; iter != end_ ; ++iter)
			{
				set.insert(*iter);
			}
			return set;
		}
		//to map
		template<typename TPredict1, typename TPredict2>
		auto to_map(const TPredict1& keySelector, const TPredict2& valueSelector) const -> std::map<decltype(keySelector(*begin_)), decltype(valueSelector(*begin_))>
		{
			std::map<decltype(keySelector(*begin_)), decltype(valueSelector(*begin_))> map;
			for (auto iter = begin_; iter != end_ ; ++iter)
			{
				map.insert(std::make_pair(keySelector(*iter), valueSelector(*iter)));
			}
			return map;
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
        template<typename TList>
        auto concat(const TList& list) const -> Queryable<iterators::concat_iter<TIterator, decltype(std::begin(list))>>
        {
            using TIterator2 = decltype(std::begin(list));
            return Queryable<iterators::concat_iter<TIterator, TIterator2>>(
                iterators::concat_iter<TIterator, TIterator2>(begin_, end_, std::begin(list), std::end(list)),
                iterators::concat_iter<TIterator, TIterator2>(end_, end_, std::end(list), std::end(list))
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
		Queryable<iterators::adapter_iter<std::shared_ptr<std::vector<TElement>>>> default_if_empty() const
		{
			return default_if_empty(TElement());
		}

		//default_if_empty with parameter
		Queryable<iterators::adapter_iter<std::shared_ptr<std::vector<TElement>>>> default_if_empty(const TElement& default_value) const
		{
			auto p = std::make_shared<std::vector<TElement>>();
			if (empty())
			{
				p->push_back(default_value);
			}
			else
			{
				for (auto iter = begin_; iter != end_; ++iter)
				{
					p->push_back(*iter);
				}
			}
			return Queryable<iterators::adapter_iter<std::shared_ptr<std::vector<TElement>>>>(
				iterators::adapter_iter<std::shared_ptr<std::vector<TElement>>>(p, p->begin(), p->end()),
				iterators::adapter_iter<std::shared_ptr<std::vector<TElement>>>(p, p->end(), p->end())
				);
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
		Queryable<iterators::adapter_iter<std::shared_ptr<std::set<TElement>>>> distinct() const
		{
			if (empty()) throw linq_exception("Empty Collection");
			auto set = std::make_shared<std::set<TElement>>();
			for (auto iter = begin_; iter != end_; ++iter)
			{
				set->insert(*iter);
			}
			return Queryable<iterators::adapter_iter<std::shared_ptr<std::set<TElement>>>>(
				iterators::adapter_iter<std::shared_ptr<std::set<TElement>>>(set, set->begin(), set->end()),
				iterators::adapter_iter<std::shared_ptr<std::set<TElement>>>(set, set->end(), set->end())
				);
		}
		//except
		template<typename TList>
		Queryable<iterators::adapter_iter<std::shared_ptr<std::vector<TElement>>>> except(const TList& l) const
		{
            auto q = from(l);
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
			return Queryable<iterators::adapter_iter<std::shared_ptr<std::vector<TElement>>>>(
				iterators::adapter_iter<std::shared_ptr<std::vector<TElement>>>(v, v->begin(), v->end()),
				iterators::adapter_iter<std::shared_ptr<std::vector<TElement>>>(v, v->end(), v->end())
				);
		}
		//intersect
		template<typename TList>
		Queryable<iterators::adapter_iter<std::shared_ptr<std::vector<TElement>>>> intersect(const TList& l) const
		{
            auto q = from(l);
			if (empty() || q.empty()) throw linq_exception("Empty Collection");
			auto v = std::make_shared<std::vector<TElement>>(std::distance(q.begin(), q.end()));
            std::set<TElement> set1(begin_, end_);
			std::set<TElement> set2(q.begin(), q.end());
            auto it = std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), v->begin());
            v->resize(it-v->begin());
			return Queryable<iterators::adapter_iter<std::shared_ptr<std::vector<TElement>>>>(
				iterators::adapter_iter<std::shared_ptr<std::vector<TElement>>>(v, v->begin(), v->end()),
				iterators::adapter_iter<std::shared_ptr<std::vector<TElement>>>(v, v->end(), v->end())
				);
		}
		//union, use linq_union to avoid key word union
		template<typename TList>
		Queryable<iterators::adapter_iter<std::shared_ptr<std::set<TElement>>>> linq_union(const TList& l) const
		{
            auto q = from(l);
			if (empty() || q.empty()) throw linq_exception("Empty Collection");
			return concat(q).distinct();
		}
		//zip
        template<typename TList>
		auto zip(const TList& l) const ->Queryable<iterators::zip_iter<TIterator, decltype(std::begin(l))>> const
		{
            using TIterator2 = decltype(std::begin(l));
			return Queryable<iterators::zip_iter<TIterator, TIterator2>>(
				iterators::zip_iter<TIterator, TIterator2>(begin_, end_, std::begin(l), std::end(l)),
				iterators::zip_iter<TIterator, TIterator2>(end_, end_, std::end(l), std::end(l)));
		}
		//order_by
        template<typename TPredict>
        auto order_by(const TPredict& keySelector) const
        {
            using TKey = decltype(keySelector(*(TElement*)0));
            return flatten(group_by(keySelector)).select([](const TElement &v) {return v;});
        }
		//group_by
		template<typename TPredict>
		auto group_by(const TPredict& keySelector) const -> std::vector<std::vector<TElement>> const
		{
			return group_by(keySelector, [](const TElement &ele) {return ele; });
		}
		//group_by with value selector
		template<typename TPredict1, typename TPredict2>
		auto group_by(const TPredict1& keySelector, const TPredict2& valueSelector) const -> std::vector<std::vector<decltype(valueSelector(*(TElement*)0))>> const
		{
			using TKey = decltype(keySelector(*(TElement*)0));
			using TValue = decltype(valueSelector(*(TElement*)0));
            std::vector<std::vector<TValue>> vv;
            std::map<TKey, std::vector<TValue>> m;

			for (auto iter = begin_; iter != end_; ++iter)
			{
                auto key = keySelector(*iter);
                auto value = valueSelector(*iter);
                if(m.find(key) != m.end())
                {
                    auto &v = m[key];
                    v.emplace_back(value);
                }
                else
                {
                    std::vector<TValue> v;
                    v.emplace_back(value);
                    m.insert(std::make_pair(key, v));
                }
			}
            for(auto& p : m)
            {
                vv.push_back(p.second);
            }
            return vv;
		}
		//join
        //
		//group_join
        //helper, flatten lists, lazy
        auto flatten(const std::vector<std::vector<TElement>> &vv) const
        {
            return from(vv).select_many([](const std::vector<TElement> &v){return from(v);});
        }
	};
}
