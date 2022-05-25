#pragma once
#include "my_memory.h"
#include <type_traits>
#include "smart_ptr.hpp"

namespace crt
{
    // specialization for functions with no arguments
    template <typename T>
    class function;

    template <typename ReturnType, typename... Args>
    class function<ReturnType(Args...)>
    {
    public:
        typedef ReturnType(*invoke_fn_t)(uint8_t* rep, Args&&...);
        typedef void (*construct_fn_t)(uint8_t* rep, uint8_t* rhs_rep);
        typedef void (*destroy_fn_t)(uint8_t* rep);

        // generic type aware function for invoking operator() with args
        template <typename Functor>
        static ReturnType invoke_fn(Functor* fn, Args&&... args)
        {
            return (*fn)(std::forward<Args>(args)...);
        }

        // generic copy constructor for two classes, in this case lambdas.
        template <typename Functor>
        static void construct_fn(Functor* construct_dst, Functor* construct_src)
        {
            new (construct_dst) Functor(*construct_src);
        }

        // generic destructor invoker for a class.
        template <typename Functor>
        static void destroy_fn(Functor* f)
        {
            f->~Functor();
        }

        // default constructor
        function() = default;

        // constructor from lambda and raw function pointer
        template <class Functor> function(Functor f)
            : repr_(crt::make_smart<uint8_t[]>(sizeof Functor))
            , repr_size_(sizeof Functor)
            , invoke_f_((invoke_fn_t)invoke_fn<Functor>)
            , construct_f_((construct_fn_t)construct_fn<Functor>)
            , destroy_f_((destroy_fn_t)destroy_fn<Functor>)
        {
            // deep copy of the underlying lambda
            construct_f_(this->repr_.get(), reinterpret_cast<uint8_t*>(&f));
        }

        // copy constructor
        function(const function& rhs)
            : repr_size_(rhs.repr_size_)
            , invoke_f_(rhs.invoke_f_)
            , construct_f_(rhs.construct_f_)
            , destroy_f_(rhs.destroy_f_)
        {
            this->repr_ = crt::make_smart<uint8_t[]>(repr_size_);
            this->construct_f_(this->repr_.get(), rhs.repr_.get());
        }

        // move constructor
        function(function&& rhs) noexcept : function()
        {
            crt::swap(this->repr_, rhs.repr_);
            crt::swap(this->repr_size_, rhs.repr_size_);
            crt::swap(this->invoke_f_, rhs.invoke_f_);
            crt::swap(this->construct_f_, rhs.construct_f_);
            crt::swap(this->destroy_f_, rhs.destroy_f_);
        }

        // copy assignment operator
        function& operator=(const function& rhs)
        {
            // free this, deep copy other.
            if (this != &rhs)
            {
                if (repr_) {
                    this->destroy_f_(this->repr_.get());
                    this->repr_.release();
                }

                repr_size_ = rhs.repr_size_;
                if (repr_size_)
                    repr_ = crt::make_smart<uint8_t[]>(repr_size_);

                invoke_f_ = rhs.invoke_f_;
                construct_f_ = rhs.construct_f_;
                destroy_f_ = rhs.destroy_f_;

                if (construct_f_)
                    construct_f_(this->repr_.get(), rhs.repr_.get());
            }

            return *this;
        }

        // todo not working
        // move assignment operator
        function& operator=(function&& rhs) noexcept
        {
            if (repr_) {
                this->destroy_f_(this->repr_.get());
                this->repr_.release();
                repr_size_ = 0;
            }
            invoke_f_ = nullptr;
            construct_f_ = nullptr;
            destroy_f_ = nullptr;


            crt::swap(this->repr_, rhs.repr_);
            crt::swap(this->repr_size_, rhs.repr_size_);
            crt::swap(this->invoke_f_, rhs.invoke_f_);
            crt::swap(this->construct_f_, rhs.construct_f_);
            crt::swap(this->destroy_f_, rhs.destroy_f_);

            return *this;
        }

        ~function()
        {
            if (repr_) {
                this->destroy_f_(this->repr_.get());
            }
        }

        ReturnType operator()(Args... args) const
        {
            CRT_ASSERT(this->repr_.get(), "Tried to call null function!");
            return this->invoke_f_(this->repr_.get(), std::forward<Args>(args)...);
        }

    private:
        // these have the same mentality of object storage class in hash table.
        // we are holding the bits representation, but will use behaviors on them.
        crt::smart_ptr<uint8_t[]> repr_{};
        size_t repr_size_{};

        // these store the behaviour of the functor
        invoke_fn_t invoke_f_{};
        construct_fn_t construct_f_{};
        destroy_fn_t destroy_f_{};
    };
}
