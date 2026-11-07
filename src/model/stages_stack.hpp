#pragma once

#include <cstdint>
#include <variant>
#include <string>
#include <deque>

#include <opencv.hpp>

#include "defs.hpp"
#include "operation.hpp"
#include "stage.hpp"

class StagesStack {
    public:
        StagesStack();

        /*void pushBase(Color transparent, Color fill, int width, int height, const std::string& file = {});
        void pushBase(const LayerBaseProps& props, const std::string& file = {});

        void push(Operation oper, Param param);*/
        void push(Op::GenericOperation& oper);
        void pop() { _container.pop_back(); }

        auto& top() { return _container.back().second; }
        const auto& top() const { return _container.back().second; }

        auto size() const { return _container.size(); }

        void populateItemList(std::vector<std::string>& items);

    private:
        std::deque<std::pair<std::string, Stage>> _container;

    private:
        void resize(int width, int height);
        void rotate(int angle);
        void loadFromFile(const std::string& file);
        void fill(uint32_t clr);
        void setTranparentColor(uint32_t clr);
        void shift(int x, int y);
        void tile(const std::string& file, int x, int y, int width, int height);

        Image getLastImage();
};
