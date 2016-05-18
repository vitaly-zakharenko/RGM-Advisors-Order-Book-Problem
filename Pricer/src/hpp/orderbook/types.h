#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <string>

namespace orderbook {

    typedef int Price;
#define PRICE_UNDEFINED -1 

    typedef unsigned Quantity;
    typedef std::string OrderId;
    typedef unsigned long Timestamp;
}
#endif /* TYPES_H */

