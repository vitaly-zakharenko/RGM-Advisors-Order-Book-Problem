#include "orderbook/LimitOrder.h"

namespace orderbook {

    LimitOrder::LimitOrder(OrderId orderId, bool isBuyOrder, Price price, Quantity quantity)
    : orderId(orderId), isBuyOrder(isBuyOrder), price(price), quantity(quantity) {

    }

    OrderId LimitOrder::getOrderId() const {
        return orderId;
    }

    bool LimitOrder::isBuy() const {
        return isBuyOrder;
    }

    Price LimitOrder::getPrice() const {
        return price;
    }

    Quantity LimitOrder::getQuantity() const {
        return quantity;
    }

    void LimitOrder::setQuantity(Quantity quantity) {
        this->quantity = quantity;
    }

}