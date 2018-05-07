#ifndef _UCLE_CORE_UTIL_SIMPLE_SHARED_PTR_HPP_
#define _UCLE_CORE_UTIL_SIMPLE_SHARED_PTR_HPP_

#include <algorithm>
#include <cstdio>  // TODO: Remove
#include <memory>
#include <type_traits>

namespace ucle::util {

    class ref_counter {
        public:
            using counter_type = long;

            ref_counter() {}
            ref_counter(const ref_counter& rc) : pn_ { rc.pn_ } {}

            void swap(ref_counter& lhs) noexcept { std::swap(pn_, lhs.pn_); }
            counter_type count() const noexcept { return pn_ ? *pn_ : 0; }

            template <typename U>
            void acquire(U* p)
            {
                if (!p) return;

                if (!pn_) {
                    try {
                        pn_ = new counter_type { 0 };  // may throw std::bad_alloc
                    } catch (std::bad_alloc&) {
                        delete p;
                        throw;  // rethrow the std::bad_alloc
                    }
                }
                
                ++(*pn_);
            }

            template<class U>
            void release(U* p) noexcept
            {
                if (!pn_) return;

                --(*pn_);

                if (*pn_ == 0) {
                    puts("deleting");
                    delete p;
                    delete pn_;
                }
                
                pn_ = nullptr;
            }

        private:
            counter_type* pn_ { nullptr };
    };

    template <typename T>
    class simple_shared_ptr {
        public:
            using element_type = T;

            simple_shared_ptr() {}            
            explicit simple_shared_ptr(T* p) : pn_ {} { acquire_(p); }
            simple_shared_ptr(const simple_shared_ptr& ptr) noexcept : pn_ { ptr.pn_ } { acquire_(ptr.px_); }

            template <typename U, typename = std::enable_if<std::is_convertible_v<U, T>>>
            simple_shared_ptr(const simple_shared_ptr<U>& ptr) noexcept : pn_ { ptr.pn_ } { acquire_(static_cast<typename simple_shared_ptr<T>::element_type*>(ptr.px_)); }

            ~simple_shared_ptr() noexcept { release_(); }

            simple_shared_ptr& operator=(simple_shared_ptr ptr) noexcept { swap(ptr); return *this; }

            void swap(simple_shared_ptr& lhs) noexcept
            {
                std::swap(px_, lhs.px_);
                pn_.swap(lhs.pn_);
            }

            void reset() noexcept { release_(); }
            void reset(T* p)  { release_(); acquire_(p); }

            auto count() const noexcept { return pn_.count(); }

            T& operator*() const noexcept { return *px_; }
            T* operator->() const noexcept { return px_; }
            T* get() const noexcept { return px_; }

        private:
            // template <typename U>
            // simple_shared_ptr(const simple_shared_ptr<U>& ptr, T* p) : pn_ { ptr.pn_ } { acquire_(p); }

            void acquire_(T* p) { pn_.acquire(p); px_ = p; }
            void release_() noexcept { pn_.release(px_); px_ = nullptr; }

            T* px_ { nullptr };
            ref_counter pn_;

            template <typename U>
            friend class simple_shared_ptr;        
    };

    template<typename U, typename V>
    inline bool operator==(const simple_shared_ptr<U>& l, const simple_shared_ptr<V>& r) noexcept { return l.get() == r.get(); }
    template<typename U, typename V>
    inline bool operator!=(const simple_shared_ptr<U>& l, const simple_shared_ptr<V>& r) noexcept { return l.get() != r.get(); }

    // template<typename T, typename U>
    // simple_shared_ptr<T> static_pointer_cast(const simple_shared_ptr<U>& ptr) noexcept
    // {
    //     return simple_shared_ptr<T>(ptr, static_cast<typename simple_shared_ptr<T>::element_type*>(ptr.get()));
    // }

    // template<typename T, typename U>
    // simple_shared_ptr<T> dynamic_pointer_cast(const simple_shared_ptr<U>& ptr) noexcept
    // {
    //     T* p = dynamic_cast<typename simple_shared_ptr<T>::element_type*>(ptr.get());
    //     return p ? simple_shared_ptr<T>(ptr, p) : simple_shared_ptr<T>();
    // }

    template<typename T, typename ...Args>
    auto make_simple_shared(Args&& ... args)
    {
        simple_shared_ptr<T> ptr { new T(args...) };
        return ptr;
    }
}

#endif  /* _UCLE_CORE_UTIL_SIMPLE_SHARED_PTR_HPP_ */