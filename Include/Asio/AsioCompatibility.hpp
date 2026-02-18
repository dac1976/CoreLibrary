// This file is part of CoreLibrary containing useful reusable utility
// classes.
//
// Copyright (C) 2014 to present, Duncan Crutchley
// Contact <dac1976github@outlook.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License and GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// and GNU Lesser General Public License along with this program. If
// not, see <http://www.gnu.org/licenses/>.

/*!
 * \file AsioCompatibility.hpp
 * \brief File containing useful ASIO definitions.
 */


#ifndef ASIO_COMPATIBILITY_HPP
#define ASIO_COMPATIBILITY_HPP

#include <boost/version.hpp>
#include <boost/asio.hpp>
#include <string>

namespace core_lib
{
namespace asio_compat
{

// -----------------------------------------------------------------------------
// io_service / io_context + strand/work-guard compatibility
// -----------------------------------------------------------------------------
#if BOOST_VERSION >= 106600
using io_context_t = boost::asio::io_context;
using executor_t   = io_context_t::executor_type;
using work_guard_t = boost::asio::executor_work_guard<executor_t>;
using strand_t     = boost::asio::strand<executor_t>;

inline work_guard_t make_work_guard(io_context_t& svc)
{
    return boost::asio::make_work_guard(svc);
}

inline strand_t make_strand(io_context_t& io)
{
    return strand_t(io.get_executor());
}

template <class F> inline void post(strand_t& s, F&& f)
{
    boost::asio::post(s, std::forward<F>(f));
}

template <class F> inline void post(io_context_t& io, F&& f)
{
    boost::asio::post(io, std::forward<F>(f));
}

template <class F> inline void dispatch(strand_t& s, F&& f)
{
    boost::asio::dispatch(s, std::forward<F>(f));
}

template <class F> inline void dispatch(io_context_t& io, F&& f)
{
    boost::asio::dispatch(io, std::forward<F>(f));
}

template <class Handler> inline auto wrap(strand_t& s, Handler&& h)
{
    return boost::asio::bind_executor(s, std::forward<Handler>(h));
}
#else
using io_context_t = boost::asio::io_service;
using work_guard_t = boost::asio::io_service::work;
using strand_t     = boost::asio::io_service::strand;

inline work_guard_t make_work_guard(io_context_t& svc)
{
    return work_guard_t(svc);
}

inline strand_t make_strand(io_context_t& io)
{
    return strand_t(io);
}

template <class F> inline void post(strand_t& s, F&& f)
{
    s.post(std::forward<F>(f));
}

template <class F> inline void post(io_context_t& io, F&& f)
{
    io.post(std::forward<F>(f));
}

template <class F> inline void dispatch(strand_t& s, F&& f)
{
    s.dispatch(std::forward<F>(f));
}

template <class F> inline void dispatch(io_context_t& io, F&& f)
{
    io.dispatch(std::forward<F>(f));
}

template <class Handler>
inline auto wrap(strand_t& s, Handler&& h)
    -> decltype(s.wrap(std::forward<Handler>(h)))
{
    return s.wrap(std::forward<Handler>(h));
}
#endif

// -----------------------------------------------------------------------------
// Address parsing compatibility
//
// Boost.Asio newer versions prefer boost::asio::ip::make_address().
// Older versions have boost::asio::ip::address::from_string().
// Provide a single call-site API that works across versions.
// -----------------------------------------------------------------------------

inline boost::asio::ip::address make_address(const std::string& s)
{
#if BOOST_VERSION >= 108700
    return boost::asio::ip::make_address(s);
#else
    return boost::asio::ip::address::from_string(s);
#endif
}

inline boost::asio::ip::address make_address(const std::string& s, boost::system::error_code& ec)
{
#if BOOST_VERSION >= 108700
    return boost::asio::ip::make_address(s, ec);
#else
    return boost::asio::ip::address::from_string(s, ec);
#endif
}

// Convenience overloads (avoid temporary std::string allocations)
inline boost::asio::ip::address make_address(const char* s)
{
#if BOOST_VERSION >= 108700
    return boost::asio::ip::make_address(s);
#else
    return boost::asio::ip::address::from_string(s);
#endif
}

inline boost::asio::ip::address make_address(const char* s, boost::system::error_code& ec)
{
#if BOOST_VERSION >= 108700
    return boost::asio::ip::make_address(s, ec);
#else
    return boost::asio::ip::address::from_string(s, ec);
#endif
}

struct udp_resolve_spec
{
    boost::asio::ip::udp protocol;
    std::string          host;
    std::string          service; // usually port as string
};

inline udp_resolve_spec make_udp_resolve_spec(boost::asio::ip::udp protocol, std::string host,
                                              std::string service)
{
    return udp_resolve_spec{protocol, std::move(host), std::move(service)};
}

#if BOOST_VERSION < 107000
using udp_resolve_results_t = boost::asio::ip::udp::resolver::iterator;
#else
using udp_resolve_results_t = boost::asio::ip::udp::resolver::results_type;
#endif

inline udp_resolve_results_t resolve_udp(boost::asio::ip::udp::resolver& r,
                                        udp_resolve_spec const&         spec,
                                        boost::system::error_code&      ec)
{
#if BOOST_VERSION < 107000
    boost::asio::ip::udp::resolver::query q(spec.protocol, spec.host, spec.service);
    return r.resolve(q, ec);                 // returns iterator
#else
    return r.resolve(spec.protocol, spec.host, spec.service, ec); // returns results_type
#endif
}

inline boost::asio::ip::udp::endpoint resolve_udp_first_endpoint(
    boost::asio::ip::udp::resolver& r,
    udp_resolve_spec const& spec,
    boost::system::error_code& ec)
{
#if BOOST_VERSION < 107000
    // old: iterator
    boost::asio::ip::udp::resolver::query q(spec.protocol, spec.host, spec.service);
    auto it = r.resolve(q, ec);
    if (ec || it == boost::asio::ip::udp::resolver::iterator{}) return {};
    return it->endpoint();
#else
    // new: results range (or similar)
    auto results = r.resolve(spec.protocol, spec.host, spec.service, ec);
    if (ec) return {};
    auto it = results.begin();
    if (it == results.end()) return {};
    return it->endpoint();
#endif
}


} // namespace asio_compat
} // namespace core_lib

#endif // ASIO_COMPATIBILITY_HPP

