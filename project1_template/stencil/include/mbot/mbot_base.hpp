#pragma once

#include "rix/msg/geometry/Twist2DStamped.hpp"

using rix::msg::geometry::Twist2DStamped;

class MBotBase {
   public:
    MBotBase() = default;
    virtual ~MBotBase() = default;

    virtual bool ok() const = 0;
    virtual void drive(const Twist2DStamped &cmd) const = 0;
};