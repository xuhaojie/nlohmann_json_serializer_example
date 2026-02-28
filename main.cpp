#include <iostream>
#include <optional>
#include <nlohmann/json.hpp>

// 1. 为std::optional<int>特化adl_serializer
namespace nlohmann {
template <>
struct adl_serializer<std::optional<int>> {
    // 序列化：将optional<int>转为JSON
    static void to_json(json& j, const std::optional<int>& opt) {
        if (opt == std::nullopt) {
            j = nullptr;  // 空值序列化为JSON null
        } else {
            j = *opt;     // 有值则直接存储整数
        }
    }

    // 反序列化：从JSON恢复optional<int>
    static void from_json(const json& j, std::optional<int>& opt) {
        if (j.is_null()) {
            opt = std::nullopt;  // JSON null对应空optional
        } else {
            opt = j.get<int>();  // 否则解析为整数
        }
    }
};
}  // namespace nlohmann

// 2. 定义包含optional成员的复杂结构体
struct UserData {
    std::string name;
    int age;
    std::optional<int> score;  // 可选分数（可能未设置）
};

// 3. 使用宏定义结构体的序列化逻辑（非侵入式）
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UserData, name, age, score)

int main() {
    // 测试数据1：包含可选分数
    UserData user1{"Alice", 25, 95};
    nlohmann::json j1 = user1;
    std::cout << "序列化带分数的用户：" << j1.dump(4) << std::endl;
    // 输出：{"age":25,"name":"Alice","score":95}

    // 测试数据2：不包含可选分数
    UserData user2{"Bob", 30, std::nullopt};
    nlohmann::json j2 = user2;
    std::cout << "序列化无分数的用户：" << j2.dump(4) << std::endl;
    // 输出：{"age":30,"name":"Bob","score":null}

    // 反序列化测试
    auto user3 = j2.get<UserData>();
    std::cout << "反序列化结果：" << user3.name << "，年龄：" << user3.age
              << "，分数：" << (user3.score.has_value() ? std::to_string(*user3.score) : "未设置") << std::endl;
    // 输出：Bob，年龄：30，分数：未设置

    return 0;
}
