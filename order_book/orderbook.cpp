#include <iostream>
#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <limits>
#include <cstring>
#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <variant>
#include <optional>
#include <tuple>
#include <format>
#include <cstdint>
#include <format>

enum class OrderType{
    GTC,
    FnK
};

enum class Side{
    buy,
    sell
};

using Price = std::int32_t;
using Quantity = std:: uint32_t;
using OrderId = std:: uint64_t;

struct LevelInfo{
    Price price_;
    Quantity quantity_;
    Quantity count_;
};

using LevelInfos = std::vector<LevelInfo>;

class OrderbookLevelInfos{
public:
    OrderbookLevelInfos(const LevelInfos& bids, const LevelInfos & asks)
        : bids_{bids}
        , asks_{asks}
     { }

    const LevelInfos& GetBids() const { return bids_;}
    const LevelInfos& GetAsks() const {return asks_; }

private:
    LevelInfos bids_;
    LevelInfos asks_;
};

class Order{
public:
    Order(OrderType orderType, OrderId orderId,  Side side, Price price, Quantity quantity)
     :orderType_{ orderType}
     ,orderId_{orderId}
     ,side_{side}
     ,price_{price}
     ,initialQuantity_{quantity}
     ,remainingQuantity_{quantity}
    { }

    OrderId GetOrderId() const { return orderId_;}
    Side GetSide() const {return side_;}
    Price GetPrice() const {return price_;}
    OrderType GetOrderType() const {return orderType_;}
    Quantity GetInitialQuantity() const { return initialQuantity_;}
    Quantity GetRemainingQuantity() const { return remainingQuantity_;}
    Quantity GetFilledQuantity() const { return GetInitialQuantity()- GetRemainingQuantity();}
    bool isFilled() const { return GetRemainingQuantity() == 0;}
    
    void Fill(Quantity quantity){
        if(quantity > GetRemainingQuantity())
            throw std::logic_error(std::format("order ({}) cant be filled for more than its remaining quantity.", GetOrderId()));
        remainingQuantity_-= quantity;
    }
private:
    OrderId orderId_;
    OrderType orderType_;
    Side side_;
    Price price_;
    Quantity initialQuantity_;
    Quantity remainingQuantity_;
};

using OrderPointer = std:: shared_ptr<Order>;
using OrderPointers = std:: list<OrderPointer>;

class OrderModify
{
public:
    OrderModify(OrderId orderId, Side side, Price price, Quantity quantity)
     : orderId_{orderId}
     ,price_{price}
     ,side_{side}
     ,quantity_{ quantity}
    { }

    OrderId GetOrderId() const {return orderId_;}
    Price GetPrice() const {return price_;}
    Side GetSide() const {return side_;}
    Quantity GetQuantity() const {return quantity_;}

    OrderPointer ToOrderPointer(OrderType type) const
    {
        return std::make_shared<Order>(type, GetOrderId(),GetSide(), GetPrice(), GetQuantity());  
    }

private:
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity quantity_;

};

struct TradeInfo{
    OrderId orderId_;
    Price price_;
    Quantity quantity_;
};

class Trade
{
private:
    TradeInfo bidTrade_;
    TradeInfo askTrade_;
public:
    Trade(const TradeInfo&bidTrade, const TradeInfo& askTrade)
        : bidTrade_{bidTrade}
        , askTrade_{askTrade}
    { }
    const TradeInfo& GetBidTrades() const{ return bidTrade_;}
    const TradeInfo& GetAskTrades() const{ return askTrade_;}

};

using Trades = std :: vector<Trade>;
class Orderbook{
private:
    struct OrderEntry{
        OrderPointer order_{ nullptr};
        OrderPointers::iterator location_;
    };

    std::map<Price, OrderPointers, std:: greater<Price>> bids_;
    std::map<Price, OrderPointers, std:: less<Price>> asks_;
    std::unordered_map<OrderId, OrderEntry> orders_;

    bool CanMatch(Side side , Price price) const
    {
        if(side == Side::buy){
            if(asks_.empty())
                return false;

            const auto& [bestAsk, _] = * asks_.begin();
            return price >= bestAsk;
        }
        else
        {
            if(bids_.empty())
                return false;

            const auto&[bestBid_, _] = *bids_.begin();
            return price<= bestBid_;
        }
    }

    Trades MatchOrders()
    {
        Trades trades;
        trades.reserve(orders_.size());
        while(true)
        {
            if(bids_.empty() || asks_.empty())
                break;
            
            auto &[bidPrice, bids] = *bids_.begin();
            auto &[askPrice, asks] = *asks_.begin();

            if(bidPrice< askPrice)
                break;

            while(bids.size() && asks.size())
            {
                auto& bid = bids.front();
                auto& ask = asks.front();

                Quantity quantity = std::min(bid->GetRemainingQuantity(),ask->GetRemainingQuantity());
                bid->Fill(quantity);
                ask->Fill(quantity);

                if(bid->isFilled())
                {
                    bids.pop_front();
                    orders_.erase(bid->GetOrderId());
                }

                if(ask->isFilled())
                {
                    asks.pop_front();
                    orders_.erase(ask->GetOrderId());
                }

                if(bids.empty()) bids_.erase(bidPrice);
                if(asks.empty()) asks_.erase(askPrice);

                trades.push_back(Trade{
                    TradeInfo{bid->GetOrderId(), bid->GetPrice(), quantity},
                    TradeInfo{ask->GetOrderId(), ask->GetPrice(), quantity}
                    });
            }
        }

        if(!bids_.empty())
        {
            auto& [_, bids] = *bids_.begin();
            auto&order = bids.front();
            if(order-> GetOrderType() == OrderType::FnK)
                CancelOrder(order-> GetOrderId());
        }
        if(!asks_.empty())
        {
            auto& [_, asks] = *asks_.begin();
            auto&order = asks.front();
            if(order-> GetOrderType() == OrderType::FnK)
                CancelOrder(order-> GetOrderId());
        }

        return trades;
    }

public:

    Trades AddOrder(OrderPointer order)
    {
        if(orders_.contains(order->GetOrderId()))
            return { };
        if(order->GetOrderType() == OrderType::FnK && !CanMatch(order->GetSide(), order->GetPrice()) )
            return { };

        OrderPointers::iterator iterator;

        if(order ->GetSide() == Side::buy)
        {
            auto& orders = bids_[order->GetPrice()];
            orders.push_back(order);
            iterator = std::next(orders.begin(),orders.size() - 1);
        }
        else
        {
            auto &orders = asks_[order->GetPrice()];
            orders.push_back(order);
            iterator = std::next(orders.begin(),orders.size() - 1);
        }
        orders_.insert({order->GetOrderId(), OrderEntry{order, iterator}});
        return MatchOrders();
    }

    void CancelOrder(OrderId orderId)
    {
        if(!orders_.contains(orderId)) return;
        const auto&[order, iterator] = orders_.at(orderId);
        orders_.erase(orderId);

        if(order->GetSide() == Side::sell)
        {
            auto price = order->GetPrice();
            auto&orders = asks_.at(price);
            orders.erase(iterator);
            if(orders.empty())
                asks_.erase(price);
        }
        else
        {
            auto price = order->GetPrice();
            auto&orders = bids_.at(price);
            orders.erase(iterator);
            if(orders.empty())
                bids_.erase(price);
        }
    }

    Trades MatchOrder(OrderModify order)
    {
        if(!orders_.contains(order.GetOrderId())) return { };

        const auto& [existingOrder, _] = orders_.at(order.GetOrderId());
        CancelOrder(order.GetOrderId());
        return AddOrder(order.ToOrderPointer(existingOrder->GetOrderType()));

    }

    std::size_t Size() const {return orders_.size();}
    OrderbookLevelInfos GetOrderInfos() const
    {
         LevelInfos bidInfos, askInfos;
         bidInfos.reserve(orders_.size());
         askInfos.reserve(asks_.size());

         auto CreateLevelInfos = [](Price price, const OrderPointers& orders) {
         return LevelInfo{ price,
            std::accumulate(orders.begin(), orders.end(), (Quantity)0,
            [](std::size_t runningSum, const OrderPointer& order)
            { return runningSum + order->GetRemainingQuantity(); }),
            (Quantity)orders.size()
        };
    };

         for(const auto& [price, orders] : bids_)
            bidInfos.push_back(CreateLevelInfos(price, orders));
         for(const auto& [price, orders] : asks_)
            askInfos.push_back(CreateLevelInfos(price, orders));

         return OrderbookLevelInfos{bidInfos,askInfos};
    }
};

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
extern "C" {
    EMSCRIPTEN_KEEPALIVE Orderbook* ob_create() { return new Orderbook(); }
    EMSCRIPTEN_KEEPALIVE void ob_destroy(Orderbook* ob) { delete ob; }

    EMSCRIPTEN_KEEPALIVE int ob_add_order(Orderbook* ob, int type, uint32_t id, int side, int32_t price, uint32_t qty) {
        auto o = std::make_shared<Order>(
            type == 0 ? OrderType::GTC : OrderType::FnK,
            id,
            side == 0 ? Side::buy : Side::sell,
            price, qty
        );
        auto trades = ob->AddOrder(o);
        return (int)trades.size();
    }

    EMSCRIPTEN_KEEPALIVE void ob_cancel(Orderbook* ob, uint32_t id) { ob->CancelOrder(id); }
    EMSCRIPTEN_KEEPALIVE int  ob_size(Orderbook* ob) { return (int)ob->Size(); }

    EMSCRIPTEN_KEEPALIVE void ob_get_levels(Orderbook* ob, char* buf, int buflen) {
    auto info = ob->GetOrderInfos();
    std::string j = "{\"bids\":[";
    for (const auto& b : info.GetBids())
        j += "{\"px\":" + std::to_string(b.price_) + 
             ",\"qty\":" + std::to_string(b.quantity_) + 
             ",\"n\":" + std::to_string(b.count_) + "},";
    if (!info.GetBids().empty()) j.pop_back();
    j += "],\"asks\":[";
    for (const auto& a : info.GetAsks())
        j += "{\"px\":" + std::to_string(a.price_) + 
             ",\"qty\":" + std::to_string(a.quantity_) + 
             ",\"n\":" + std::to_string(a.count_) + "},";
    if (!info.GetAsks().empty()) j.pop_back();
    j += "]}";
    strncpy(buf, j.c_str(), buflen - 1);
    buf[buflen - 1] = '\0';
  }
}
#endif

int main(){
    Orderbook orderbook;
    orderbook.AddOrder(std::make_shared<Order>(OrderType::GTC, 1, Side::buy, 100, 10));
    orderbook.AddOrder(std::make_shared<Order>(OrderType::GTC, 2, Side::sell, 100, 5));
    auto trades = orderbook.AddOrder(std::make_shared<Order>(OrderType::GTC, 3, Side::buy, 105, 8));
    std::cout << "Trades: " << trades.size() << "\n";
    std::cout << "Orders remaining: " << orderbook.Size() << "\n";
    return 0;
}