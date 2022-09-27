#pragma once

#include <string_view>

namespace R {
    enum class id;
}

namespace rosy::resources {
    extern auto get_string(R::id id) -> std::string_view;
}