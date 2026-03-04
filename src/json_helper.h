#pragma once
#include <nlohmann/json.hpp>
#include <optional>
#include <type_traits>

// 当使用 NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE 或相关宏时，
// 如果结构体包含 std::optional 字段，库会自动使用此序列化器。（存在缺省字段则解析失败）
namespace nlohmann {

template <typename T>
struct adl_serializer<std::optional<T>> {
    static void to_json(json &j, const std::optional<T> &opt) {
        if (opt == std::nullopt) {
            j = nullptr;
        } else {
            j = *opt;
        }
    }

    static void from_json(const json &j, std::optional<T> &opt) {
        if (j.is_null()) {
            opt = std::nullopt;
        } else {
            opt = j.get<T>();
        }
    }
};

} // namespace nlohmann

namespace json_utils {

// 安全获取字段值：如果字段不存在或为 null，返回默认值
template <typename T>
T safe_get(const nlohmann::json &j, const std::string &key, const T &default_value = T{}) {
    // 使用 find() 而不是 operator[]，因为 find() 不会在字段不存在时抛出异常
    auto it = j.find(key);
    // 双重检查：字段存在 且 不为 null
    if (it != j.end() && !it->is_null()) {
        return it->get<T>();
    }
    return default_value;
}

// 安全获取 optional 字段值：如果字段不存在或为 null，返回 std::nullopt
template <typename T>
std::optional<T> safe_get_optional(const nlohmann::json &j, const std::string &key) {
    auto it = j.find(key);
    if (it != j.end() && !it->is_null()) {
        return it->get<T>();
    }
    return std::nullopt;
}

} // namespace json_utils

namespace json_utils {

// 类型特征：判断类型 T 是否为 std::optional<U>
template <typename T>
struct is_optional : std::false_type {};

// 部分特化：当 T 是 std::optional<U> 时，继承 true_type
template <typename T>
struct is_optional<std::optional<T>> : std::true_type {};

// 便捷的类型特征值：is_optional_v<T> 等价于 is_optional<T>::value
template <typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

// 处理普通类型（非 optional）的辅助函数
// SFINAE 条件：!is_optional<T>::value（即不是 optional 类型）
// 如果字段存在且不为 null，则设置值；否则保持原值不变
//
template <typename T>
typename std::enable_if<!is_optional<typename std::remove_reference<T>::type>::value, void>::type
safe_set_field_from_json(const nlohmann::json &j, const std::string &key, T &value) {
    auto it = j.find(key);
    if (it != j.end() && !it->is_null()) {
        it->get_to(value);
    }
    // 注意：如果字段不存在，不修改 value，保持其默认值
}

// 处理 optional 类型的辅助函数
// SFINAE 条件：is_optional<T>::value（即是 optional 类型）
// 如果字段存在且不为 null，则设置值；否则设置为 std::nullopt
//
template <typename T>
typename std::enable_if<is_optional<typename std::remove_reference<T>::type>::value, void>::type
safe_set_field_from_json(const nlohmann::json &j, const std::string &key, T &value) {
    // 提取 optional 的内部类型（例如 std::optional<int> -> int）
    using OptionalType = typename std::remove_reference<T>::type;

    auto it = j.find(key);
    if (it != j.end() && !it->is_null()) {
        // 从 JSON 解析出内部类型，然后赋值给 optional
        value = it->get<typename OptionalType::value_type>();
    } else {
        // 字段不存在或为 null，设置为空 optional
        value = std::nullopt;
    }
}

} // namespace json_utils

// NLOHMANN_JSON_FROM_SAFE_AUTO (处理单个字段的反序列化）
#define NLOHMANN_JSON_FROM_SAFE_AUTO(v1) \
    json_utils::safe_set_field_from_json(nlohmann_json_j, #v1, nlohmann_json_t.v1);

// ============================================================================
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_SAFE
// ============================================================================
//
// 【使用示例】
//
//   1. 定义结构体：
//      struct MyData {
//          int id;                          // 必需字段
//          std::optional<bool> enable;     // 可选字段
//          std::optional<float> strength;   // 可选字段
//      };
//
//   2. 定义序列化：
//      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_SAFE(MyData, id, enable, strength);
//
//   3. 使用反序列化：
//      nlohmann::json j = R"({"id": 123, "enable": true})"_json;
//      MyData data = j.get<MyData>();
//      // data.id = 123
//      // data.enable = true (有值)
//      // data.strength = std::nullopt (字段缺失，设为空)
//
//   4. 使用序列化：
//      MyData data;
//      data.id = 456;
//      data.enable = false;
//      data.strength = std::nullopt;
//      nlohmann::json j = data;
//      // j = {"id": 456, "enable": false, "strength": null}
//
#define NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_SAFE(Type, ...)                                   \
    inline void to_json(nlohmann::json &nlohmann_json_j, const Type &nlohmann_json_t) {      \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__))             \
    }                                                                                        \
    inline void from_json(const nlohmann::json &nlohmann_json_j, Type &nlohmann_json_t) {    \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM_SAFE_AUTO, __VA_ARGS__)) \
    }
