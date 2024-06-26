#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <initializer_list>
#include <string>
#include <utility>
#include <limits>
#include <string_view>
#include <cstddef>
#include <cstring>
#include "iterator.hpp"

constexpr float MAX_LOAD_FACTOR{ 0.8f };
constexpr float MIN_LOAD_FACTOR{ 0.2f };

class hashtable 
{
public:
    class forward_iterator;
    class const_forward_iterator;

public:
    using key_type        = std::string;
    using mapped_type     = AutoDocs; 
    using value_type      = std::pair<key_type, mapped_type>;
    using size_type       = std::size_t; 
    using reference       = value_type&;
    using const_reference = const value_type&; 
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using iterator        = forward_iterator;
    using const_iterator  = const_forward_iterator;

private:
    void reallocate() 
    {
        size_type new_capacity{ m_capacity * 2 };
        pointer new_buckets{ new value_type[new_capacity]{} };
        bool* new_status{ new bool[new_capacity]{} };

        for (size_type i{}; i < m_capacity; ++i)
        {
            if (m_buckets[i].first != key_type{})
            {
                size_type index{ hash(m_buckets[i].first, new_capacity) };
                while (new_buckets[index].first != key_type{})
                {
                    index = (index + 1) % new_capacity;
                }
                new_buckets[index] = m_buckets[i];
                new_status[index] = 1;
            }
        }
        
        delete[] m_buckets;
        delete[] m_status;
        
        m_buckets = new_buckets;
        m_status = new_status;
        m_capacity = new_capacity; 
    }
    // reallocate() for reserve() method
    void reallocate(size_type capacity) 
    {
        pointer new_buckets{ new value_type[capacity]{} };
        bool* new_status{ new bool[capacity]{} };

        for (size_type i{}; i < m_capacity; ++i)
        {
            if (m_buckets[i].first != key_type{})
            {
                size_type index{ hash(m_buckets[i].first, capacity) };
                while (new_buckets[index].first != key_type{})
                {
                    index = (index + 1) % capacity;
                }
                new_buckets[index] = m_buckets[i];
                new_status[index] = 1;
            }
        }
        
        delete[] m_buckets;
        delete[] m_status;
        
        m_buckets = new_buckets;
        m_status = new_status;
        m_capacity = capacity; 
    }
    size_type hash(key_type const& key, size_type capacity) const 
    {
    	size_type result{};
        size_type fullname_sum{};
        size_type request_sum{};

        size_type i{};
    	size_type len{ key.length() };
        for (i = 0; i < len; ++i) 
        {
            if (std::isspace(key[i])) 
            {
                continue;
            }
            if (std::isdigit(key[i])) 
            {
                break;
            }
            fullname_sum += key[i];
        }

        for ( ; i < len; ++i) 
        {
            request_sum = request_sum * 10 + (key[i] - '0');
        }
        
        size_type temp{ request_sum + fullname_sum };
        
        int last, prelast;
        while (temp > 0) 
        {
            last = temp % 10;
            prelast = (temp % 100) / 10;
            result = result + (last + prelast);
            temp /= 100;
        }
        return result % capacity;
    }
    size_type _hash(size_type i, key_type const& key, size_type capacity) const 
    {
        while (m_buckets[i].first != key_type{} && m_buckets[i].first != key) 
        {
            i++;
            i %= capacity;
        }
        return i;
    }

public:
    class const_forward_iterator
    {
    public:
        const_forward_iterator(const_pointer ptr) :
            m_ptr{ ptr } 
        {}

        const_reference operator*() const noexcept
        {
            return *m_ptr;
        }
        const_pointer operator->() const noexcept
        {
            return m_ptr;
        }
        const_iterator& operator++() noexcept 
        {
            ++m_ptr;
            return *this;
        }
        const_iterator operator++(int) noexcept
        {
            const_iterator temp{ *this };
            ++(*this);
            return temp;
        }
        const_iterator operator--() noexcept
        {
            --m_ptr;
            return *this;
        }
        const_iterator operator--(int) noexcept
        {
            const_iterator temp{ *this };
            --(*this);
            return temp;
        }

        bool operator<(const_iterator const& other) const noexcept
        {
            return m_ptr - other.m_ptr < 0; 
        }
        bool operator>(const_iterator const& other) const noexcept
        {
            return m_ptr - other.m_ptr > 0;
        }
        bool operator==(const_iterator const& other) const noexcept
        {
            return m_ptr == other.m_ptr;
        } 
        bool operator!=(const_iterator const& other) const noexcept
        {
            return !(m_ptr == other.m_ptr); 
        }

    private:
        const_pointer m_ptr;
    };

    class forward_iterator
    {
    public:
        forward_iterator(pointer ptr) :
            m_ptr{ ptr }
        {}

        reference operator*() noexcept
        {
            return *m_ptr;
        }
        pointer operator->() noexcept
        {
            return m_ptr;
        }
        iterator& operator++() noexcept 
        {
            ++m_ptr;
            return *this;
        }
        iterator operator++(int) noexcept
        {
            iterator temp{ *this };
            ++(*this);
            return temp;
        }
        iterator operator--() noexcept
        {
            --m_ptr;
            return *this;
        }
        iterator operator--(int) noexcept
        {
            iterator temp{ *this };
            --(*this);
            return temp;
        }
        
        bool operator<(iterator const& other) const noexcept
        {
            return m_ptr - other.m_ptr < 0; 
        }
        bool operator>(iterator const& other) const noexcept
        {
            return m_ptr - other.m_ptr > 0;
        }
        bool operator==(iterator const& other) const noexcept
        {
            return m_ptr == other.m_ptr;
        } 
        bool operator!=(iterator const& other) const noexcept 
        {
            return !(m_ptr == other.m_ptr); 
        }
    private:
        pointer m_ptr;
    };

public:
    // constructors and destructors
    hashtable() :
        hashtable(8)
    {}
    hashtable(size_type N) :
        m_buckets{ new value_type[N]{} }, 
        m_status{ new bool[N]{} }, 
        m_size{}, 
        m_capacity{ N } 
    {}
    hashtable(std::initializer_list<value_type> ls) :
        m_buckets{ new value_type[ls.size()] }, 
        m_status{ new bool[ls.size()]{} }, 
        m_size{ ls.size() }, 
        m_capacity{ ls.size() } 
    {
        for (const_reference value : ls)
        {
            this->insert(value);
        }
    }
    ~hashtable() 
    {
        delete[] m_buckets;
        delete[] m_status;
    }
    
    //iterators
    iterator begin() noexcept 
    {
        return iterator{ m_buckets };
    } 
    iterator end() noexcept
    {
        return iterator{ m_buckets + m_capacity };
    }
    const_iterator begin() const noexcept 
    {
        return const_iterator{ m_buckets };
    }
    const_iterator end() const noexcept 
    {
        return const_iterator{ m_buckets + m_capacity };
    }

    //capacity
    bool empty() const noexcept
    {
        return !m_size;
    }
    size_type size() const noexcept
    {
        return m_size;
    }
    size_type max_size() const noexcept
    {
        return std::numeric_limits<std::size_t>::max() / sizeof(value_type);
    }
    size_type capacity() const noexcept
    {
        return m_capacity;
    }
    
    // modifiers
    std::pair<forward_iterator, bool> insert(value_type const& value) 
    {   
        if ((static_cast<float>(m_size) / static_cast<float>(m_capacity)) >= MAX_LOAD_FACTOR)
        {
            reallocate();
        }
        bool insertion_took_place{};
        size_type index{ hash(value.first, m_capacity) };
        if (m_buckets[index].first != key_type{}) 
        {
            if (m_buckets[index].first == value.first)
            {
                m_buckets[index].second = value.second;
                return { &m_buckets[index], insertion_took_place = false };
            }
            index = _hash(index, value.first, m_capacity);
        }
        m_buckets[index] = value;
        m_status[index] = 1;
        m_size++;
        return { &m_buckets[index], insertion_took_place = true };
    }    
    std::pair<forward_iterator, bool> emplace(value_type && value) 
    {   
        if ((static_cast<float>(m_size) / static_cast<float>(m_capacity)) >= MAX_LOAD_FACTOR)
        {
            reallocate();
        }
        bool insertion_took_place{};
        size_type index{ hash(value.first, m_capacity) };
        if (m_buckets[index].first != key_type{}) 
        {
            if (m_buckets[index].first == value.first)
            {
                m_buckets[index].second = std::move(value.second);
                return { &m_buckets[index], insertion_took_place = false };
            }
            index = _hash(index, value.first, m_capacity);
        }
        m_buckets[index] = std::move(value);
        m_status[index] = 1;
        m_size++;
        return { &m_buckets[index], insertion_took_place = true };
    }    
    iterator erase(iterator pos)
    {
        std::ptrdiff_t dist{ distance(this->begin(), pos) };

        size_type index{};
        index = dist < 0 ? -dist : dist;

        m_buckets[index] = value_type{};
        m_status[index] = 0;
        m_size--;
        return ++pos;
    }
    void clear() noexcept
    {
        delete[] m_buckets;
        delete[] m_status;
        *this = hashtable();
    }

    // lookup
    mapped_type& operator[](key_type const& key) 
    {
        if ((static_cast<float>(m_size) / static_cast<float>(m_capacity)) >= MAX_LOAD_FACTOR)
        {
            reallocate();
        }

        size_type index{ hash(key, m_capacity) };
        if (m_buckets[index].first != key_type{}) {
            if (m_buckets[index].first == key) 
            {
                return m_buckets[index].second;
            }
            index = _hash(index, key, m_capacity);
        }
        
        if (m_buckets[index].first == key_type{})
        {
            m_buckets[index] = value_type{ key, mapped_type{} };
            m_status[index] = 1;
            m_size++;
        }
        return m_buckets[index].second;
    }
    size_type count(key_type const& key) const
    {
        size_type count{};
        auto iter{ this->begin() };
        while (iter != this->end()) 
        {
            if (iter->first == key) 
            {
                count++;
            }
            iter++;
        }
        return count;
    }
    iterator find(key_type const& key)
    {
        size_type index{ hash(key, m_capacity) };

        if (m_buckets[index].first == key) 
        {
            return iterator{ &m_buckets[index] };
        }

        index = _hash(index, key, m_capacity);
        if (m_buckets[index] != value_type{}) 
        {
            return iterator{ &m_buckets[index] };
        }
        return this->end();
    }
    iterator find(mapped_type& value) 
    {
        std::string key{ value.getFullName().getName() + ' ' + value.getFullName().getSurname() + ' ' + std::to_string(value.getRequest()) };
        size_type index{ hash(key, m_capacity) };

        if (m_buckets[index].second == value) 
        {
            return iterator{ &m_buckets[index] };
        }

        index = _hash(index, key, m_capacity);
        if (m_buckets[index] != value_type{}) 
        {
            return iterator{ &m_buckets[index] };
        }
        return this->end();
    }
    // C++20 vvvv
    bool contains(key_type& key)
    {
        return this->find(key) != this->end();
    }

    // hash policy
    void reserve(size_type count)
    {
        if (count > this->max_size())
        {
            throw std::length_error("not enough memory");
        }

        size_type new_capacity{ count };
        pointer new_buckets{ new value_type[new_capacity]{} };
        bool* new_status{ new bool[new_capacity]{} };

        for (size_type i{}; i < m_capacity; ++i)
        {
            if (m_buckets[i].first != key_type{})
            {
                size_type index{ hash(m_buckets[i].first, new_capacity) };
                while (new_buckets[index].first != key_type{})
                {
                    index = (index + 1) % new_capacity;
                }
                new_buckets[index] = m_buckets[i];
                new_status[index] = 1;
            } 
        }
    }
    
    friend std::ostream& operator<<(std::ostream& out, hashtable const& table)
    {
        for (const_reference bucket : table)
        {
            if (bucket.first != key_type{})
            {
                size_type len{ strlen(bucket.first.c_str()) };
                out << bucket.first;
                while (len++ != 30) {
                    out << ' ';
                }
                out << '\t';
                out << '{' << bucket.second << '}' << '\n';
            }
        }
        return out;
    } 
    
private:
    pointer      m_buckets;
    bool*        m_status;
    size_type    m_size;
    size_type    m_capacity;   
};

#endif // HASHTABLE_HPP
