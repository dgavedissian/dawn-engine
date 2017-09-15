/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {
// Type safe handles.
template <typename Tag, int Invalid> class Handle {
public:
    using base_type = u16;

    static const base_type invalid = static_cast<base_type>(Invalid);

    Handle() : internal_handle_{invalid} {
    }

    explicit Handle(base_type internal_handle) : internal_handle_{internal_handle} {
    }

    explicit operator base_type() const {
        return internal();
    }

    Handle<Tag, Invalid>& operator=(base_type other) {
        internal_handle_ = other;
        return *this;
    }

    bool operator==(const Handle<Tag, Invalid>& other) const {
        return internal_handle_ == other.internal_handle_;
    }

    bool operator==(base_type other) const {
        return internal_handle_ == other;
    }

    bool operator!=(const Handle<Tag, Invalid>& other) const {
        return internal_handle_ != other.internal_handle_;
    }

    bool operator!=(base_type other) const {
        return internal_handle_ != other;
    }

    Handle<Tag, Invalid>& operator++() {
        ++internal_handle_;
        return *this;
    }

    Handle<Tag, Invalid> operator++(int) {
        Handle<Tag, Invalid> tmp{*this};
        ++internal_handle_;
        return tmp;
    }

    base_type internal() const {
        return internal_handle_;
    }

    bool isValid() const {
        return internal_handle_ != invalid;
    }

private:
    base_type internal_handle_;
};

// Handle generator.
template <typename Handle> class HandleGenerator {
public:
    HandleGenerator() : next_{1} {
    }
    ~HandleGenerator() = default;

    Handle next() {
        return next_++;
    }

private:
    Handle next_;
};
}  // namespace dw

namespace std {
template <typename Tag, int Invalid> struct hash<dw::Handle<Tag, Invalid>> {
    typedef dw::Handle<Tag, Invalid> argument_type;
    typedef std::size_t result_type;
    result_type operator()(const argument_type& k) const {
        std::hash<typename dw::Handle<Tag, Invalid>::base_type> base_hash;
        return base_hash(k.internal());
    }
};
}  // namespace std
