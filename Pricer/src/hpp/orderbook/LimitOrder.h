#ifndef ORDER_H
#define ORDER_H

#include "orderbook/types.h"

namespace orderbook {

    class LimitOrder {
    public:
        LimitOrder(OrderId orderId, bool isBuyOrder, Price price, Quantity quantity);

        OrderId getOrderId() const;

        bool isBuy() const;

        Price getPrice() const;

        Quantity getQuantity() const;
        void setQuantity(Quantity quantity);

    private:
        const OrderId orderId;
        const bool isBuyOrder;
        const Price price;

        Quantity quantity;
    };

}
#endif /* ORDER_H */

