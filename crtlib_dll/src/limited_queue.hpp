#pragma once


namespace crt
{

	// data structure that holds most recent X samples. allows us to do mathematical operations such as weighed average.
    template <typename T, size_t N>
    class limited_queue
    {
    public:
        void feed_data(const T& value)
        {
            if (empty_) [[unlikely]]
                empty_ = false;

            auto place = buffer_ + oldest_sample_;
            *place = value;

            newest_sample_ = oldest_sample_;
            ++oldest_sample_;

            if (!finished_tour_) [[unlikely]]
            {
                if (oldest_sample_ == N)
                    finished_tour_ = true;
            }

                // circular attribute
                if (oldest_sample_ >= N)
                {
                    oldest_sample_ = 0;
                }
        }

        // newest sample -> oldest sample
        struct lru_iterator
        {
            friend class limited_queue;

            using value_type = T;
            using pointer = const T*;
            using reference = const T&;

            lru_iterator(const T* buffer, size_t idx, bool tour) : buffer_(buffer), index_(idx), tour_(tour)
            {

            }

            reference operator*() const { return *(buffer_ + index_); }
            pointer operator->() { return buffer_ + index_; }

            lru_iterator& operator++()
            {
                if (index_ == 0)
                {
                    // circular
                    tour_ = true;
                    index_ = N - 1;
                }
                else
                {
                    // linear
                    --index_;
                }

                return *this;
            }

            lru_iterator operator++(int)
            {
                lru_iterator tmp = *this; ++(*this); return tmp;
            }

            friend bool operator== (const lru_iterator& a, const lru_iterator& b) { return a.index_ == b.index_ && a.tour_ == b.tour_; };
            friend bool operator!= (const lru_iterator& a, const lru_iterator& b) { return !(a == b); }

        private:
            const T* buffer_;
            size_t index_;
            bool tour_;
        };

        // oldest sample -> newest sample
        struct mru_iterator
        {
            friend class limited_queue;

            using value_type = T;
            using pointer = const T*;
            using reference = const T&;

            mru_iterator(const T* buffer, size_t idx, bool tour) : buffer_(buffer), index_(idx), tour_(tour)
            {

            }

            reference operator*() const { return *(buffer_ + index_); }
            pointer operator->() { return buffer_ + index_; }

            mru_iterator& operator++()
            {
                if (index_ == N - 1)
                {
                    // circular
                    tour_ = true;
                    index_ = 0;
                }
                else
                {
                    // linear
                    ++index_;
                }

                return *this;
            }

            mru_iterator operator++(int)
            {
                lru_iterator tmp = *this; ++(*this); return tmp;
            }

            friend bool operator== (const mru_iterator& a, const mru_iterator& b) { return a.index_ == b.index_ && a.tour_ == b.tour_; };
            friend bool operator!= (const mru_iterator& a, const mru_iterator& b) { return !(a == b); }

        private:
            const T* buffer_;
            size_t index_;
            bool tour_;
        };

        lru_iterator lru_begin() const
        {
            return lru_iterator(buffer_, newest_sample_, empty_);
        }

        lru_iterator lru_end() const
        {
            if (finished_tour_ || empty_) [[likely]]
                return lru_iterator(buffer_, newest_sample_, true);

            return lru_iterator(buffer_, N - 1, true);
        }

        mru_iterator mru_begin() const
        {
            if (finished_tour_) [[likely]]
                return mru_iterator(buffer_, oldest_sample_, empty_);

            return mru_iterator(buffer_, 0, false);
        }

        mru_iterator mru_end() const
        {
            if (finished_tour_) [[likely]]
                return mru_iterator(buffer_, oldest_sample_, true);

            return mru_iterator(buffer_, oldest_sample_, false);
        }

    private:
        T buffer_[N]{};

        size_t newest_sample_{};
        size_t oldest_sample_{};
        bool empty_{ true };
        bool finished_tour_{};
    };


}