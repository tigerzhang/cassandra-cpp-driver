#include <algorithm>
#include <exception>
#include <limits>
#include <cmath>

#include "cql/policies/cql_exponential_reconnection_policy_t.hpp"

boost::posix_time::time_duration
cql::cql_exponential_reconnection_schedule_t::get_delay() 
{
    boost::posix_time::time_duration delay = 
        _policy.base_delay() * (1 << _attempts);
    
    if(delay < _last_delay) {
        // overflown
        return _policy.max_delay();
    }
    
    _attempts++;
    _last_delay = delay;
    
    return std::min(_policy.max_delay(), delay);
}

cql::cql_exponential_reconnection_policy_t::cql_exponential_reconnection_policy_t(
        const boost::posix_time::time_duration& base_delay, 
        const boost::posix_time::time_duration& max_delay)
        : _base_delay(base_delay), _max_delay(max_delay)
{
    if(base_delay.is_negative())
        throw std::invalid_argument("base_delay cannot be negative.");
    
    if(max_delay.is_negative())
        throw std::invalid_argument("max_delay cannot be negative.");
    
    if(base_delay.total_milliseconds() == 0)
        throw std::invalid_argument("base_delay must be at least 1 milisecond long.");
    
    if(max_delay < base_delay)
        throw std::invalid_argument("base_delay cannot be greater than max_delay");
}

boost::shared_ptr<cql::cql_reconnection_schedule_t>
cql::cql_exponential_reconnection_policy_t::new_schedule()
{
    return boost::shared_ptr<cql_reconnection_schedule_t>(
        new cql_exponential_reconnection_schedule_t(*this));
}