#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <math.h>   

#include "orderbook/LimitOrder.h"
#include "orderbook/LimitOrderBookStateMonitor.h"

using namespace std;

#define MONITORED_QUANTITY 200

void buyingPriceChangeCallback(orderbook::Timestamp timestamp, orderbook::Price);
void sellingPriceChangeCallback(orderbook::Timestamp timestamp, orderbook::Price);

int main() {
    //instantiate LOB State Monitor 
    orderbook::LimitOrderBookStateMonitor lobStateMonitor(MONITORED_QUANTITY);

    //register callbacks 
    lobStateMonitor.registerBuyingPriceChangeCallback(buyingPriceChangeCallback);
    lobStateMonitor.registerSellingPriceChangeCallback(sellingPriceChangeCallback);

    //read and parse lines from STDIN and generate add/reduce calls 
    string line;
    while (getline(cin, line)) {
        std::istringstream iss(line);

        orderbook::Timestamp timestamp;
        char operation;
        orderbook::OrderId orderId;
        char side;
        double doublePrice;
        orderbook::Quantity quantity;

        if (!(iss >> timestamp >> operation >> orderId)) {
            return 1;
        }

        if (operation == 'A') {
            if (!(iss >> side >> doublePrice >> quantity)) {
                return 1;
            }

            bool isBuyOrder;
            if (side == 'B') {
                isBuyOrder = true;
            } else if (side == 'S') {
                isBuyOrder = false;
            } else {
                return 1;
            }

            orderbook::Price price = round(doublePrice * 100);
            orderbook::LimitOrder order(orderId, isBuyOrder, price, quantity);

            //add order to the LOB State Monitor
            if (!lobStateMonitor.add(timestamp, order)) {
                return 1;
            };

        } else if (operation == 'R') {
            if (!(iss >> quantity)) {
                return 1;
            }

            //reduce order in the LOB State Monitor
            if (!lobStateMonitor.reduce(timestamp, orderId, quantity)) {
                return 1;
            }
        } else {
            return 1;
        }
    }
    return 0;
}

void buyingPriceChangeCallback(orderbook::Timestamp timestamp, orderbook::Price price) {
    cout << timestamp << " B ";
    if (price > 0) {
        cout << std::fixed << std::setprecision(2) << (double) price / 100.0 << endl;
    } else {
        cout << "NA" << endl;
    }
}

void sellingPriceChangeCallback(orderbook::Timestamp timestamp, orderbook::Price price) {
    cout << timestamp << " S ";
    if (price > 0) {
        cout << std::fixed << std::setprecision(2) << (double) price / 100.0 << endl;
    } else {
        cout << "NA" << endl;
    }
}

