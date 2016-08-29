#ifndef HALIDE_BUFFER_H
#define HALIDE_BUFFER_H

/** \file
 * Defines Buffer - A c++ wrapper around a buffer_t.
 */

#include "runtime/HalideImage.h"
#include "Expr.h"
#include "Util.h"
#include "Argument.h"

namespace Halide {
namespace Internal {
struct BufferContents;

/** A named reference-counted handle on an Image of unknown type and dimensionality */
class Buffer {
private:
    Internal::IntrusivePtr<Internal::BufferContents> contents;

public:
    Buffer() : contents(nullptr) {}
    EXPORT Buffer(const Image<void> &buf, std::string name = "");
    EXPORT Buffer(Type t, const buffer_t &buf, std::string name = "");

    template<typename T, int D> Buffer(const Image<T, D> &buf, std::string name = "") :
        Buffer(Image<void>(buf), name) {}

    EXPORT Buffer(Type t, const std::vector<int> &size, std::string name = "");

    /** Compare two buffers for identity (not equality of data). */
    bool same_as(const Buffer &other) const;

    /** Get the underlying Image */
    EXPORT Image<void> &get();
    EXPORT const Image<void> &get() const;

    /** Check if this buffer handle actually points to data. */
    EXPORT bool defined() const;

    /** Get the runtime name of this buffer used for debugging. */
    EXPORT const std::string &name() const;

    /** Convert this buffer to an argument to a halide pipeline. */
    EXPORT operator Argument() const;

    /** Get the Halide type of the underlying buffer */
    EXPORT Type type() const;

    /** Get the dimensionality of the underlying buffer */
    EXPORT int dimensions() const;

    /** Get a dimension from the underlying buffer. */
    EXPORT Image<void>::Dimension dim(int i) const;

    /** Access to the mins, strides, extents. Will be deprecated. Do not use. */
    // @{
    int min(int i) const { return dim(i).min(); }
    int extent(int i) const { return dim(i).extent(); }
    int stride(int i) const { return dim(i).stride(); }
    // @}

    /** Get the size in bytes of the allocation */
    EXPORT size_t size_in_bytes() const;

    /** Get a pointer to the raw buffer */
    EXPORT buffer_t *raw_buffer() const;

    /** Get the host pointer */
    EXPORT uint8_t *host_ptr() const;

    /** Convert a buffer to a typed and dimensioned Image. Does
     * runtime type checks. */
    template<typename T, int D>
    operator Image<T, D>() const {
        return Image<T, D>(get());
    }

    /** Make a Call node to a specific site in this buffer. */
    // @{
    Expr operator()(const std::vector<Expr> &loc) const;

    template<typename ...Args,
             typename = std::enable_if<(Internal::all_are_convertible<Expr, Args...>::value)>>
    Expr operator()(Expr first, Args... rest) const {
        const std::vector<Expr> vec = {first, rest...};
        return (*this)(vec);
    }
    // @}
};

}

/** An adaptor so that it's possible to access a Halide::Image using Exprs. */
template<typename T, int D, typename ...Args,
         typename = std::enable_if<(Internal::all_are_convertible<Expr, Args...>::value)>>
NO_INLINE Expr image_accessor(const Image<T, D> &im, Expr first, Args... rest) {
    return Internal::Buffer(im)(first, rest...);
}

}

#endif
