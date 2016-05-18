#ifndef LimitOrderBookStateMonitor_H
#define LimitOrderBookStateMonitor_H

#include <map>
#include <functional>

#include "orderbook/LimitOrder.h"
#include "types.h"

namespace orderbook {

    class LimitOrderBookStateMonitor {
    public:
        LimitOrderBookStateMonitor(Quantity monitoredQuantity);

        /**
         * Update monitor state for the addition of a new limit order to the Limit Order Book
         * 
         * @param timestamp 
         * @param order              Limit order added to the LOB 
         * @return                   True if successful (added for consistency with the reduce operation)
         */
        bool add(Timestamp timestamp, const LimitOrder& order);

        /**
         * Update monitor state for the size reduction of an existing order in the Limit Order Book
         * 
         * @param timestamp 
         * @param orderId            The unique string that identifies the order to be reduced.
         * @param reductionQuantity  The amount by which to reduce the size of the order. 
         *                           If size is equal to or greater than the existing size of the order, 
         *                           the order is removed from the book.
         * @return                   True if successful, false otherwise
         */
        bool reduce(Timestamp timestamp, OrderId orderId, Quantity reductionQuantity);

        /**
         * Register a callback to be executed on the buying price change for
         * the monitored quantity specified in the constructor 
         * 
         * @param callback : Callback to be executed on lowest buying price change.
         *                   The callback is supplied with the timestamp of the change 
         *                   and the new lowest buying price for the monitored quantity.
         */
        void registerBuyingPriceChangeCallback(std::function<void(Timestamp, Price) > callback);

        /**
         * Register a callback to be executed on the selling price change for
         * the monitored quantity specified in the constructor 
         * 
         * @param callback : Callback to be executed on the selling price change.
         *                   The callback is supplied with the timestamp of the change 
         *                   and the new selling price for the monitored quantity.
         */
        void registerSellingPriceChangeCallback(std::function<void(Timestamp, Price) > callback);

    private:

        //RB tree of buy orders sorted in descending order of limit prices (high bids first)
        typedef std::multimap<Price, LimitOrder, std::greater<Price> > BuyOrderMap;

        //RB tree of sell orders sorted in ascending order of limit prices (low asks first)
        typedef std::multimap<Price, LimitOrder, std::less<Price> > SellOrderMap;

        //RB tree of all orders sorted in ascending order of limit prices 
        typedef std::map<OrderId, LimitOrder> OrderMap;

        static BuyOrderMap::iterator findBuyOrder(BuyOrderMap& buyOrders, Price price, OrderId orderId);
        static SellOrderMap::iterator findSellOrder(SellOrderMap& sellOrders, Price price, OrderId orderId);

        static Price findBestBuyingPrice(const SellOrderMap& sellOrders, Quantity monitoredQuantity);
        static Price findBestSellingPrice(const BuyOrderMap& buyOrders, Quantity monitoredQuantity);

        void processBuyOrdersChange(Timestamp timestamp);
        void processSellOrdersChange(Timestamp timestamp);

        const Quantity monitoredQuantity;

        std::function<void(Timestamp, Price) > buyingPriceChangeCallback;
        std::function<void(Timestamp, Price) > sellingPriceChangeCallback;

        BuyOrderMap buyOrders;
        SellOrderMap sellOrders;
        OrderMap orders;

        Price bestBuyingPrice;
        Price bestSellingPrice;
    };
}
#endif /* LimitOrderBookStateMonitor_H */

