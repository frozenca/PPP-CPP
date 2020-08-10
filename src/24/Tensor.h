#ifndef PPP_TENSOR_H
#define PPP_TENSOR_H

#include <algorithm>
#include <array>
#include <cassert>
#include <complex>
#include <cstddef>
#include <functional>
#include <iostream>
#include <initializer_list>
#include <numeric>
#include <utility>
#include <type_traits>
#include <valarray>

template <typename... Args>
inline constexpr bool All(Args... args) { return (... && args);};

template <typename... Args>
inline constexpr bool Some(Args... args) { return (... || args);};

template <typename R>
concept Scalar = std::is_arithmetic_v<R> ||
        std::is_same_v<R, std::complex<float>> ||
        std::is_same_v<R, std::complex<double>> ||
        std::is_same_v<R, std::complex<long double>>;

template <Scalar R, std::size_t N>
struct TensorInitializer {
    using type = std::initializer_list<typename TensorInitializer<R, N - 1>::type>;
};

template <Scalar R>
struct TensorInitializer<R, 1> {
    using type = std::initializer_list<R>;
};

template <Scalar R>
struct TensorInitializer<R, 0>;

template <typename... Args>
concept RequestingElement = All(std::is_convertible_v<Args, std::size_t>...);

template <typename... Args>
concept RequestingSlice = All((std::is_convertible_v<Args, std::size_t> || std::is_same_v<Args, std::slice>)...)
        || Some(std::is_same_v<Args, std::slice>...);

template <Scalar R, std::size_t N>
class TensorView;

template <Scalar R, std::size_t N>
class Tensor {
public:
    static constexpr std::size_t ndim = N;

    using value_type = R;
    using iterator = R;
    using const_iterator = const R*;

    iterator begin() { return std::begin(data_);}
    const_iterator begin() const { return std::begin(data_);}
    iterator end() { return std::end(data_);}
    const_iterator end() const { return std::end(data_);}

    Tensor() = delete;
    Tensor(const Tensor&) = default;
    Tensor& operator=(const Tensor&) = default;
    Tensor(Tensor&&) noexcept = default;
    Tensor& operator=(Tensor&&) noexcept = default;
    ~Tensor() = default;

    template <Scalar U>
    Tensor(const TensorView<U, N>&);
    template <Scalar U>
    Tensor& operator=(const TensorView<U, N>&);

    template <typename U>
    Tensor(std::initializer_list<U>) = delete;
    template <typename U>
    Tensor& operator=(std::initializer_list<U>) = delete;

    template <typename... Dims>
    explicit Tensor(Dims... dims);

    Tensor(typename TensorInitializer<R, N>::type init);
    Tensor& operator=(typename TensorInitializer<R, N>::type init);

    [[nodiscard]] std::size_t size() const { return data_.size();}
    [[nodiscard]] std::size_t dim(std::size_t n) const { static_assert(n < N); return dims_[n]; }

    template <RequestingElement... Args>
    R& operator()(Args... args);

    template <RequestingElement... Args>
    const R& operator()(Args... args) const;

    template <RequestingSlice... Args>
    TensorView<R, N>& operator()(const Args&... args);

    template <RequestingSlice... Args>
    TensorView<const R, N>& operator()(const Args&... args) const;

    TensorView<R, N - 1> operator[](std::size_t i) {return row(i);}
    TensorView<const R, N - 1> operator[](std::size_t i) const  {return row(i);}

    TensorView<R, N - 1> row(std::size_t n);
    TensorView<const R, N - 1> row(std::size_t n) const;

    std::enable_if_t<(N > 1), TensorView<R, N - 1>> col(std::size_t n);
    std::enable_if_t<(N > 1), TensorView<const R, N - 1>> col(std::size_t n) const;

    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator=(const U& val);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator+=(const U& val);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator-=(const U& val);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator*=(const U& val);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator/=(const U& val);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator%=(const U& val);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator&=(const U& val);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator|=(const U& val);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator^=(const U& val);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator<<=(const U& val);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator>>=(const U& val);

    [[nodiscard]] R sum() const { return data_.sum(); }
    [[nodiscard]] R min() const { return data_.min(); }
    [[nodiscard]] R max() const { return data_.max(); }

    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator+=(const Tensor<U, N>& v);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator-=(const Tensor<U, N>& v);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator*=(const Tensor<U, N>& v);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator/=(const Tensor<U, N>& v);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator%=(const Tensor<U, N>& v);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator&=(const Tensor<U, N>& v);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator|=(const Tensor<U, N>& v);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator^=(const Tensor<U, N>& v);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator<<=(const Tensor<U, N>& v);
    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor&> operator>>=(const Tensor<U, N>& v);

    template <typename F>
    Tensor& apply(F f);

private:
    std::array<std::size_t, N> dims_;
    std::valarray<R> data_;
    std::array<std::size_t, N> strides_;

    void compute_strides(const std::array<std::size_t, N>& a);
    template <typename... Dims>
    void compute_strides(Dims... dims);

    template <std::size_t D, typename Initializer>
    bool check_non_jagged(const Initializer& init);

    template <std::size_t D, typename Iter, typename Initializer>
    void add_dims(Iter& first, const Initializer& init);

    template <typename Initializer>
    void derive_dims(std::array<std::size_t, N>& dims, const Initializer& init);

    template <typename T>
    void insert_flat(std::initializer_list<T> list);

    template <typename T>
    void add_list(const std::initializer_list<T>* first, const std::initializer_list<T>* last, std::size_t& index);

    template <typename T>
    void add_list(const T* first, const T* last, std::size_t& index);

    template <std::size_t D>
    std::size_t do_slice_dim(std::gslice& gs, std::slice s);

    template <std::size_t D>
    std::size_t do_slice_dim(std::gslice& gs, std::size_t s);

    std::size_t do_slice(std::gslice& gs) {
        return 0;
    }

    template <typename T, typename... Args>
    std::size_t do_slice(std::gslice& gs, const T& s, Args&&... args);

    friend class TensorView<R, N>;

};

template <Scalar R, std::size_t N>
void Tensor<R, N>::compute_strides(const std::array<size_t, N>& a) {
    std::size_t str = 1;
    for (int i = N - 1; i >= 0; i--) {
        strides_[i] = str;
        str *= a[i];
    }
}

template <Scalar R, std::size_t N>
template <typename... Dims>
void Tensor<R, N>::compute_strides(Dims... dims) {
    static_assert(sizeof...(Dims) == N);
    std::array<std::size_t, N> a{dims...};
    compute_strides(a);
}

template <Scalar R, std::size_t N>
template <typename... Dims>
Tensor<R, N>::Tensor(Dims... dims) : dims_{dims...}, data_((... * dims)) {
    static_assert(sizeof...(Dims) == N);
    compute_strides(dims...);
}

template <Scalar R, std::size_t N>
template <std::size_t D, typename Initializer>
bool Tensor<R, N>::check_non_jagged(const Initializer& init) {
    auto i = init.begin();
    for (auto j = i + 1; j != init.end(); ++j) {
        if (i->size() != j->size()) {
            return false;
        }
    }
    return true;
}

template <Scalar R, std::size_t N>
template <std::size_t D, typename Iter, typename Initializer>
void Tensor<R, N>::add_dims(Iter& first, const Initializer& init) {
    if constexpr (D > 1)
        assert(check_non_jagged<D>(init));
    *first++ = init.size();
    if constexpr (D > 1)
        add_dims<D - 1>(first, *init.begin());
}

template <Scalar R, std::size_t N>
template <typename Initializer>
void Tensor<R, N>::derive_dims(std::array<std::size_t, N>& dims, const Initializer& init) {
    auto f = dims.begin();
    add_dims<N>(f, init);
}

template <Scalar R, std::size_t N>
template <typename T>
void Tensor<R, N>::add_list(const std::initializer_list<T>* first, const std::initializer_list<T>* last, std::size_t& index) {
    for (; first != last; ++first) {
        add_list(first->begin(), first->end(), index);
    }
}

template <Scalar R, std::size_t N>
template <typename T>
void Tensor<R, N>::add_list(const T* first, const T* last, std::size_t& index) {
    for (; first != last; ++first) {
        data_[index++] = *first;
    }
}

template <Scalar R, std::size_t N>
template <typename T>
void Tensor<R, N>::insert_flat(std::initializer_list<T> list) {
    std::size_t index = 0;
    add_list(list.begin(), list.end(), index);
}

template <Scalar R, std::size_t N>
Tensor<R, N>::Tensor(typename TensorInitializer<R, N>::type init) {
    derive_dims(dims_, init);
    compute_strides(dims_);
    data_.resize(strides_[0] * dims_[0]);
    insert_flat(init);
}

template <Scalar R, std::size_t N>
template <RequestingElement... Args>
R& Tensor<R, N>::operator()(Args... args) {
    static_assert(sizeof...(args) == N);
    std::size_t pos[N] { std::size_t(args)...};
    assert(std::equal(pos, pos + N, dims_.begin(), std::less<std::size_t>{}));
    return data_[std::inner_product(pos, pos + N, strides_.begin(), std::size_t{0})];
}

template <Scalar R, std::size_t N>
template <RequestingElement... Args>
const R& Tensor<R, N>::operator()(Args... args) const {
    static_assert(sizeof...(args) == N);
    std::size_t pos[N] { std::size_t(args)...};
    assert(std::equal(pos, pos + N, dims_.begin(), std::less<std::size_t>{}));
    return data_[std::inner_product(pos, pos + N, strides_.begin(), std::size_t{0})];
}

template <Scalar R, std::size_t N>
template <std::size_t D>
std::size_t Tensor<R, N>::do_slice_dim(std::gslice& gs, std::slice s) {
    assert(s.start() < dims_[D] && s.size() + s.start() <= dims_[D] && s.size() <= dims_[D]
    && s.start() + s.size() * s.stride() <= dims_[D]);

    gs.size()[D] = s.size();
    gs.stride()[D] = s.stride() * strides_[D];

    return s.start() * strides_[D];
}

template <Scalar R, std::size_t N>
template <std::size_t D>
std::size_t Tensor<R, N>::do_slice_dim(std::gslice& gs, std::size_t s) {
    return do_slice_dim<D>(gs, std::slice {s, 1, 1});
}

template <Scalar R, std::size_t N>
template <typename T, typename... Args>
std::size_t Tensor<R, N>::do_slice(std::gslice& gs, const T& s, Args&&... args) {
    constexpr std::size_t D = N - sizeof...(Args) - 1;
    std::size_t m = do_slice_dim<D>(gs, s);
    std::size_t n = do_slice(gs, args...);
    return m + n;
}

template <Scalar R, std::size_t N>
template<RequestingSlice... Args>
TensorView<R, N>& Tensor<R, N>::operator()(const Args&... args) {
    std::gslice gs;
    gs.start() = do_slice(gs, args...);
    return {gs, data_};
}

template <Scalar R, std::size_t N>
template<RequestingSlice... Args>
TensorView<const R, N>& Tensor<R, N>::operator()(const Args&... args) const {
    std::gslice gs;
    gs.start() = do_slice(gs, args...);
    return {gs, data_};
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
        Tensor<R, N>::operator=(const U& val) {
    data_ = val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
Tensor<R, N>::operator+=(const U& val) {
    data_ += val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
Tensor<R, N>::operator-=(const U& val) {
    data_ -= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
Tensor<R, N>::operator*=(const U& val) {
    data_ *= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
Tensor<R, N>::operator/=(const U& val) {
    data_ /= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
Tensor<R, N>::operator%=(const U& val) {
    data_ %= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
Tensor<R, N>::operator&=(const U& val) {
    data_ &= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
Tensor<R, N>::operator|=(const U& val) {
    data_ |= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
Tensor<R, N>::operator^=(const U& val) {
    data_ ^= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
Tensor<R, N>::operator<<=(const U& val) {
    data_ <<= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
Tensor<R, N>::operator>>=(const U& val) {
    data_ >>= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
        Tensor<R, N>::operator+=(const Tensor<U, N>& v) {
    assert(data_.size() == v.data_.size());
    data_ += v.data_;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
        Tensor<R, N>::operator-=(const Tensor<U, N>& v) {
    assert(data_.size() == v.data_.size());
    data_ -= v.data_;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
        Tensor<R, N>::operator*=(const Tensor<U, N>& v) {
    assert(data_.size() == v.data_.size());
    data_ *= v.data_;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
        Tensor<R, N>::operator/=(const Tensor<U, N>& v) {
    assert(data_.size() == v.data_.size());
    data_ /= v.data_;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
        Tensor<R, N>::operator%=(const Tensor<U, N>& v) {
    assert(data_.size() == v.data_.size());
    data_ %= v.data_;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
        Tensor<R, N>::operator&=(const Tensor<U, N>& v) {
    assert(data_.size() == v.data_.size());
    data_ &= v.data_;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
        Tensor<R, N>::operator|=(const Tensor<U, N>& v) {
    assert(data_.size() == v.data_.size());
    data_ |= v.data_;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
        Tensor<R, N>::operator^=(const Tensor<U, N>& v) {
    assert(data_.size() == v.data_.size());
    data_ ^= v.data_;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
        Tensor<R, N>::operator<<=(const Tensor<U, N>& v) {
    assert(data_.size() == v.data_.size());
    data_ <<= v.data_;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, Tensor<R, N>&>
        Tensor<R, N>::operator>>=(const Tensor<U, N>& v) {
    assert(data_.size() == v.data_.size());
    data_ >>= v.data_;
    return *this;
}

template <Scalar R, std::size_t N>
template <typename F>
Tensor<R, N>& Tensor<R, N>::apply(F f) {
    for (auto& x : data_) {
        f(x);
    }
    return *this;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator+ (const Tensor<R1, N>& lhs, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = lhs;
    res += rhs;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator- (const Tensor<R1, N>& lhs, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = lhs;
    res -= rhs;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator* (const Tensor<R1, N>& lhs, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = lhs;
    res *= rhs;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator/ (const Tensor<R1, N>& lhs, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = lhs;
    res /= rhs;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator% (const Tensor<R1, N>& lhs, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = lhs;
    res %= rhs;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator& (const Tensor<R1, N>& lhs, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = lhs;
    res &= rhs;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator| (const Tensor<R1, N>& lhs, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = lhs;
    res |= rhs;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator^ (const Tensor<R1, N>& lhs, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = lhs;
    res ^= rhs;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator<< (const Tensor<R1, N>& lhs, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = lhs;
    res <<= rhs;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator>> (const Tensor<R1, N>& lhs, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = lhs;
    res >>= rhs;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator+ (const Tensor<R1, N>& lhs, const R2& val) {
    Tensor<R3, N> res = lhs;
    res += val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator- (const Tensor<R1, N>& lhs, const R2& val) {
    Tensor<R3, N> res = lhs;
    res -= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator* (const Tensor<R1, N>& lhs, const R2& val) {
    Tensor<R3, N> res = lhs;
    res *= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator/ (const Tensor<R1, N>& lhs, const R2& val) {
    Tensor<R3, N> res = lhs;
    res /= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator% (const Tensor<R1, N>& lhs, const R2& val) {
    Tensor<R3, N> res = lhs;
    res %= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator& (const Tensor<R1, N>& lhs, const R2& val) {
    Tensor<R3, N> res = lhs;
    res &= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator| (const Tensor<R1, N>& lhs, const R2& val) {
    Tensor<R3, N> res = lhs;
    res |= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator^ (const Tensor<R1, N>& lhs, const R2& val) {
    Tensor<R3, N> res = lhs;
    res ^= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator<< (const Tensor<R1, N>& lhs, const R2& val) {
    Tensor<R3, N> res = lhs;
    res <<= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator>> (const Tensor<R1, N>& lhs, const R2& val) {
    Tensor<R3, N> res = lhs;
    res >>= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator+ (const R1& val, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = rhs;
    res += val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator- (const R1& val, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = rhs;
    res -= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator* (const R1& val, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = rhs;
    res *= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator/ (const R1& val, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = rhs;
    res /= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator% (const R1& val, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = rhs;
    res %= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator& (const R1& val, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = rhs;
    res &= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator| (const R1& val, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = rhs;
    res |= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator^ (const R1& val, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = rhs;
    res ^= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator<< (const R1& val, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = rhs;
    res <<= val;
    return res;
}

template <Scalar R1, Scalar R2, std::size_t N, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, N> operator>> (const R1& val, const Tensor<R2, N>& rhs) {
    Tensor<R3, N> res = rhs;
    res >>= val;
    return res;
}

template <Scalar R, std::size_t N>
class TensorView {
public:
    static constexpr std::size_t ndim = N;

    TensorView() = delete;
    TensorView(const TensorView&) = default;
    TensorView& operator=(const TensorView&) = default;
    TensorView(TensorView&&) noexcept = default;
    TensorView& operator=(TensorView&&) noexcept = default;
    ~TensorView() = default;

    template <typename U>
    TensorView(std::initializer_list<U>) = delete;
    template <typename U>
    TensorView& operator=(std::initializer_list<U>) = delete;

    TensorView(const std::gslice& gsl, std::valarray<R>& data);

    template <RequestingElement... Args>
    R& operator()(Args... args);

    template <RequestingElement... Args>
    const R& operator()(Args... args) const;

    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView&> operator=(const U& val);

    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView&> operator+=(const U& val);

    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView&> operator-=(const U& val);

    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView&> operator*=(const U& val);

    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView&> operator/=(const U& val);

    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView&> operator%=(const U& val);

    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView&> operator&=(const U& val);

    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView&> operator|=(const U& val);

    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView&> operator^=(const U& val);

    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView&> operator<<=(const U& val);

    template <Scalar U>
    std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView&> operator>>=(const U& val);

private:
    std::gslice_array<R> data_view_;
    std::array<std::size_t, N> size_;
    std::array<std::size_t, N> strides_;
    R* ptr;

    friend class Tensor<R, N>;
};

template <Scalar R, std::size_t N>
TensorView<R, N>::TensorView(const std::gslice& gsl, std::valarray<R>& data) : data_view_ {data[gsl]}{
    assert(gsl.size().size() == gsl.stride().size() && gsl.size().size() == N);
    for (std::size_t i = 0; i < N; i++) {
        size_[i] = gsl.size()[i];
        strides_[i] = gsl.stride()[i];
    }
    ptr = &data[gsl.start()];
}

template <Scalar R, std::size_t N>
template <Scalar U>
Tensor<R, N>::Tensor(const TensorView<U, N>& view) : data_ {view.data_view_} {
    static_assert(std::is_convertible_v<U, R>);
    std::size_t str = 1;
    for (int i = N - 1; i >= 0; i--) {
        dims_[i] = view.size_[i];
        strides_[i] = str;
        str *= dims_[i];
    }
}

template <Scalar R, std::size_t N>
template <Scalar U>
Tensor<R, N>& Tensor<R, N>::operator=(const TensorView<U, N>& view) {
    static_assert(std::is_convertible_v<U, R>);
    data_ (view.data_view_);
    std::size_t str = 1;
    for (int i = N - 1; i >= 0; i--) {
        dims_[i] = view.size_[i];
        strides_[i] = str;
        str *= dims_[i];
    }
    return *this;
}

template <Scalar R, std::size_t N>
TensorView<R, N - 1> Tensor<R, N>::row(std::size_t n) {
    assert(n < dims_[0]);
    std::size_t start = strides_[0] * n;
    std::valarray<std::size_t> sizes (N - 1);
    std::valarray<std::size_t> strides (N - 1);

    std::copy(dims_.begin() + 1, dims_.end(), std::begin(sizes));
    std::copy(strides_.begin() + 1, strides_.end(), std::begin(strides));
    TensorView<R, N - 1> view (std::gslice(start, sizes, strides), data_);
    return view;
}

template <Scalar R, std::size_t N>
TensorView<const R, N - 1> Tensor<R, N>::row(std::size_t n) const {
    assert(n < dims_[0]);
    std::size_t start = strides_[0] * n;
    std::valarray<std::size_t> sizes (N - 1);
    std::valarray<std::size_t> strides (N - 1);

    std::copy(dims_.begin() + 1, dims_.end(), std::begin(sizes));
    std::copy(strides_.begin() + 1, strides_.end(), std::begin(strides));
    TensorView<const R, N - 1> view (std::gslice(start, sizes, strides), data_);
    return view;
}

template <Scalar R, std::size_t N>
std::enable_if_t<(N > 1), TensorView<R, N - 1>> Tensor<R, N>::col(std::size_t n) {
    assert(n < dims_[1]);
    std::size_t start = strides_[1] * n;
    std::valarray<std::size_t> sizes (N - 1);
    std::valarray<std::size_t> strides (N - 1);

    std::copy(dims_.begin(), dims_.begin() + 1, std::begin(sizes));
    std::copy(strides_.begin(), strides_.begin() + 1, std::begin(strides));
    std::copy(dims_.begin() + 2, dims_.end(), std::begin(sizes) + 1);
    std::copy(strides_.begin() + 2, strides_.end(), std::begin(strides) + 1);
    TensorView<R, N - 1> view (std::gslice(start, sizes, strides), data_);
    return view;
}

template <Scalar R, std::size_t N>
std::enable_if_t<(N > 1), TensorView<const R, N - 1>> Tensor<R, N>::col(std::size_t n) const {
    assert(n < dims_[1]);
    std::size_t start = strides_[1] * n;
    std::valarray<std::size_t> sizes (N - 1);
    std::valarray<std::size_t> strides (N - 1);

    std::copy(dims_.begin(), dims_.begin() + 1, std::begin(sizes));
    std::copy(strides_.begin(), strides_.begin() + 1, std::begin(strides));
    std::copy(dims_.begin() + 2, dims_.end(), std::begin(sizes) + 1);
    std::copy(strides_.begin() + 2, strides_.end(), std::begin(strides) + 1);
    TensorView<const R, N - 1> view (std::gslice(start, sizes, strides), data_);
    return view;
}

template <Scalar R, std::size_t N>
template <RequestingElement... Args>
R& TensorView<R, N>::operator()(Args... args) {
    static_assert(sizeof...(args) == N);
    std::size_t pos[N] { std::size_t(args)...};
    assert(std::equal(pos, pos + N, size_.begin(), std::less<std::size_t>{}));
    return *(ptr + std::inner_product(pos, pos + N, strides_.begin(), std::size_t{0}));
}

template <Scalar R, std::size_t N>
template <RequestingElement... Args>
const R& TensorView<R, N>::operator()(Args... args) const {
    static_assert(sizeof...(args) == N);
    std::size_t pos[N] { std::size_t(args)...};
    assert(std::equal(pos, pos + N, size_.begin(), std::less<std::size_t>{}));
    return *(ptr + std::inner_product(pos, pos + N, strides_.begin(), std::size_t{0}));
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView<R, N>&>
        TensorView<R, N>::operator=(const U& val) {
    data_view_ = val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView<R, N>&>
TensorView<R, N>::operator+=(const U& val) {
    data_view_ += val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView<R, N>&>
TensorView<R, N>::operator-=(const U& val) {
    data_view_ -= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView<R, N>&>
TensorView<R, N>::operator*=(const U& val) {
    data_view_ *= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView<R, N>&>
TensorView<R, N>::operator/=(const U& val) {
    data_view_ /= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView<R, N>&>
TensorView<R, N>::operator%=(const U& val) {
    data_view_ %= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView<R, N>&>
TensorView<R, N>::operator&=(const U& val) {
    data_view_ &= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView<R, N>&>
TensorView<R, N>::operator|=(const U& val) {
    data_view_ |= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView<R, N>&>
TensorView<R, N>::operator^=(const U& val) {
    data_view_ ^= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView<R, N>&>
TensorView<R, N>::operator<<=(const U& val) {
    data_view_ <<= val;
    return *this;
}

template <Scalar R, std::size_t N>
template <Scalar U>
std::enable_if_t<std::is_convertible_v<U, R> && std::is_convertible_v<R, U>, TensorView<R, N>&>
TensorView<R, N>::operator>>=(const U& val) {
    data_view_ >>= val;
    return *this;
}

template <Scalar R1, Scalar R2, Scalar R3 = std::common_type_t<R1, R2>>
Tensor<R3, 2> operator* (const Tensor<R1, 2>& A, const Tensor<R2, 2>& B) {
    assert(A.dim(1) == B.dim(0));
    const std::size_t M = A.dim(0);
    const std::size_t K = A.dim(1);
    const std::size_t N = B.dim(1);

    Tensor<R3, 2> C (M, N);

    for (std::size_t i = 0; i < M; i++) {
        for (std::size_t j = 0; j < N; j++) {
            for (std::size_t k = 0; k < K; k++) {
                C(i) += A(i, k) * B(k, j);
            }
        }
    }
    return C;
}


#endif //PPP_TENSOR_H
