#pragma once

#include <string_view>

namespace R {
    enum class id : __SIZE_TYPE__;
}

namespace rosy::resources {
    extern std::string_view get_string(const R::id id);
}