#include "orderbook/LimitOrderBookStateMonitor.h"

using namespace std;

namespace orderbook {

    LimitOrderBookStateMonitor::LimitOrderBookStateMonitor(Quantity monitoredQuantity)
    : monitoredQuantity(monitoredQuantity) {
        bestBuyingPrice = PRICE_UNDEFINED;
        bestSellingPrice = PRICE_UNDEFINED;
    }

    bool LimitOrderBookStateMonitor::add(Timestamp timestamp, const LimitOrder& order) {
        orders.insert(make_pair(order.getOrderId(), order));

        pair<Price, LimitOrder> priceOrderPair = make_pair(order.getPrice(), order);
        if (order.isBuy()) {
            buyOrders.insert(priceOrderPair);
            processBuyOrdersChange(timestamp);
        } else {
            sellOrders.insert(priceOrderPair);
            processSellOrdersChange(timestamp);
        }
        return true;
    }

    bool LimitOrderBookStateMonitor::reduce(Timestamp timestamp, OrderId orderId, Quantity reductionQuantity) {
        OrderMap::iterator orderIter = orders.find(orderId);
        if (orderIter == orders.end()) {
            return false;
        }
        LimitOrder& order = orderIter->second;
        Quantity newQuantity = order.getQuantity() - reductionQuantity;

        if (order.isBuy()) {
            BuyOrderMap::iterator buyOrderIter = findBuyOrder(buyOrders, order.getPrice(), order.getOrderId());
            if (buyOrderIter == buyOrders.end()) {
                return false;
            }
            LimitOrder& buyOrder = buyOrderIter->second;

            if (newQuantity <= 0) {
                orders.erase(orderIter);
                buyOrders.erase(buyOrderIter);
            } else {
                order.setQuantity(newQuantity);
                buyOrder.setQuantity(newQuantity);
            }
            processBuyOrdersChange(timestamp);
        } else {
            SellOrderMap::iterator sellOrderIter = findSellOrder(sellOrders, order.getPrice(), order.getOrderId());
            if (sellOrderIter == sellOrders.end()) {
                return false;
            }
            LimitOrder& sellOrder = sellOrderIter->second;

            if (newQuantity <= 0) {
                orders.erase(orderIter);
                sellOrders.erase(sellOrderIter);
            } else {
                order.setQuantity(newQuantity);
                sellOrder.setQuantity(newQuantity);
            }
            processSellOrdersChange(timestamp);
        }
        return true;
    }

    LimitOrderBookStateMonitor::BuyOrderMap::iterator LimitOrderBookStateMonitor::findBuyOrder(BuyOrderMap& buyOrders, Price price, OrderId orderId) {
        BuyOrderMap::iterator result;
        for (result = buyOrders.find(price); result != buyOrders.end(); ++result) {
            if (result->second.getOrderId() == orderId) {
                return result;
            } else if (result->first < price) {
                break;
            }
        }
        return buyOrders.end();
    }

    LimitOrderBookStateMonitor::SellOrderMap::iterator LimitOrderBookStateMonitor::findSellOrder(SellOrderMap& sellOrders, Price price, OrderId orderId) {
        SellOrderMap::iterator result;
        for (result = sellOrders.find(price); result != sellOrders.end(); ++result) {
            if (result->second.getOrderId() == orderId) {
                return result;
            } else if (result->first > price) {
                break;
            }
        }
        return sellOrders.end();
    }

    void LimitOrderBookStateMonitor::registerBuyingPriceChangeCallback(std::function<void(Timestamp, Price) > callback) {
        this->buyingPriceChangeCallback = callback;
    }

    void LimitOrderBookStateMonitor::registerSellingPriceChangeCallback(std::function<void(Timestamp, Price) > callback) {
        this->sellingPriceChangeCallback = callback;
    }

    Price LimitOrderBookStateMonitor::findBestBuyingPrice(const SellOrderMap& sellOrders, Quantity monitoredQuantity) {
        Price price = 0;
        Quantity remainingQuantity = monitoredQuantity;
        for (SellOrderMap::const_iterator sellOrderIter = sellOrders.begin(); sellOrderIter != sellOrders.end(); ++sellOrderIter) {
            Quantity availableQuantity = sellOrderIter->second.getQuantity();
            if (availableQuantity >= remainingQuantity) {
                price += sellOrderIter->second.getPrice() * remainingQuantity;
                remainingQuantity = 0;
                break;
            }
            price += sellOrderIter->second.getPrice() * availableQuantity;
            remainingQuantity -= availableQuantity;
        }
        if (remainingQuantity == 0) {
            return price;
        }
        return PRICE_UNDEFINED;
    }

    Price LimitOrderBookStateMonitor::findBestSellingPrice(const BuyOrderMap& buyOrders, Quantity monitoredQuantity) {
        Price price = 0;
        Quantity remainingQuantity = monitoredQuantity;
        for (BuyOrderMap::const_iterator buyOrderIter = buyOrders.begin(); buyOrderIter != buyOrders.end(); ++buyOrderIter) {
            Quantity availableQuantity = buyOrderIter->second.getQuantity();
            if (availableQuantity >= remainingQuantity) {
                price += buyOrderIter->second.getPrice() * remainingQuantity;
                remainingQuantity = 0;
                break;
            }
            price += buyOrderIter->second.getPrice() * availableQuantity;
            remainingQuantity -= availableQuantity;
        }
        if (remainingQuantity == 0) {
            return price;
        }
        return PRICE_UNDEFINED;
    }

    void LimitOrderBookStateMonitor::processBuyOrdersChange(Timestamp timestamp) {
        Price newBestSellingPrice = findBestSellingPrice(buyOrders, monitoredQuantity);
        if (newBestSellingPrice == bestSellingPrice) {
            return;
        }

        bestSellingPrice = newBestSellingPrice;
        if (sellingPriceChangeCallback) {
            sellingPriceChangeCallback(timestamp, bestSellingPrice);
        }
    }

    void LimitOrderBookStateMonitor::processSellOrdersChange(Timestamp timestamp) {
        Price newBestBuyingPrice = findBestBuyingPrice(sellOrders, monitoredQuantity);
        if (newBestBuyingPrice == bestBuyingPrice) {
            return;
        }

        bestBuyingPrice = newBestBuyingPrice;
        if (buyingPriceChangeCallback) {
            buyingPriceChangeCallback(timestamp, bestBuyingPrice);
        }
    }
}