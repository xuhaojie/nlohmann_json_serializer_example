#include <iostream>
#include <optional>
#include "json_helper.h"

struct UserData {
    std::string name;
    std::optional<int> age;    // optional, maybe absent
    std::optional<int> score;  // optional, maybe absent
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_SAFE(UserData, name, age, score)

bool parse_json(const std::string& json_str, UserData& user) {
    try {
        nlohmann::json j = nlohmann::json::parse(json_str);
        j.get_to(user);
        return true;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    }
}

void apply_data(const UserData& user) {
    std::cout << "Result:" << std::endl;
    std::cout << "name: " << user.name << std::endl;
    std::cout << "age: " << (user.age.has_value() ? std::to_string(*user.age) : "none") << std::endl;
    std::cout << "score: " << (user.score.has_value() ? std::to_string(*user.score) : "none") << std::endl;
}

int main() {
    // Test case 1: all fields present
    // std::string json_str = R"({"name":"Alice","age":25,"score":95})";
    
    // Test case 2: null value
    // std::string json_str = R"({"name":"Alice","age":null})";
    
    // Test case 3: missing optional fields
    std::string json_str = R"({"name":"Alice"})";
    
    UserData data;
    if (!parse_json(json_str, data)) {
        return 1;
    }
    apply_data(data);
    return 0;
}
