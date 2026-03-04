#include <iostream>
#include <optional>
#include "json_helper.h"

struct UserData {
    std::string name;
    std::optional<int> age; // optional,maybe absent
    std::optional<int> score;  // optional,maybe absent
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_SAFE(UserData, name, age, score)

int parse_json(const std::string& json_str, UserData& user) {
    nlohmann::json j = nlohmann::json::parse(json_str);
    j.get_to(user);
    return 0;
}

int apply_data(const UserData& user) {
    std::cout << "Result：" << std::endl;
    std::cout << "name：" << user.name << std::endl;
    std::cout << "age：" << (user.age.has_value() ? std::to_string(*user.age) : "none") << std::endl;
    std::cout << "score：" << (user.score.has_value() ? std::to_string(*user.score) : "none") << std::endl;
    return 0;
}

int main() {
    // std::string json_str = R"({"name":"Alice","age":25,"score":95})";
    // std::string json_str = R"({"name":"Alice","age":null})";
    std::string json_str = R"({"name":"Alice"})";
    UserData data;
    parse_json(json_str, data);
    apply_data(data);
    return 0;
}
