#include <iostream>
#include <optional>
#include "json_helper.h"

struct UserData {
    std::string name;
    int age;
    std::optional<int> score;  // 可选分数（可能未设置）
};

// 3. 使用宏定义结构体的序列化逻辑（非侵入式）
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_SAFE(UserData, name, age, score)

int parse_json(const std::string& json_str, UserData& user) {
    nlohmann::json j = nlohmann::json::parse(json_str);
    j.get_to(user);
    return 0;
}

int apply_data(const UserData& user) {
    // 反序列化测试
    std::cout << "反序列化结果：" << user.name << "，年龄：" << user.age
              << "，分数：" << (user.score.has_value() ? std::to_string(*user.score) : "未设置") << std::endl;
    // 输出：Bob，年龄：30，分数：未设置
    return 0;
}

int test() {
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
}

int main() {
    // 测试数据2：不包含可选分数
    //std::string json_str = R"({"age":25,"name":"Alice","score":95})";
    std::string json_str = R"({"age":25,"name":"Alice"})";
    UserData data;
    parse_json(json_str, data);
    apply_data(data);
    return 0;
}
