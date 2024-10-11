//
// Created by Xsakura on 2023/4/26.
//

#include <fstream>
#include <iostream>

#include "crow.h"
#include "crow/middlewares/cors.h"

int main() {
  std::ifstream file(R"(D:\Users\Xsakura\Desktop\test_data\ms_1.json)");
  if (!file.is_open()) {
    std::cerr << "Failed to open file." << std::endl;
    return 1;
  }
  // 对于规范的 json 格式文件直接读取就好了😂
  std::string json_str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  //  rapidjson::Document data;
  //  if (data.Parse(json_str.c_str()).HasParseError()) {
  //    std::cerr << "Cant' parse this file." << std::endl;
  //  }
  //  rapidjson::StringBuffer buffer;
  //  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  //  data.Accept(writer);

  crow::App<crow::CORSHandler> app;
  // 配置中间层处理 cors
  auto& cors = app.get_middleware<crow::CORSHandler>();
  cors.global()
      .origin("*")
      .headers("origin, x-requested-with, accept, access-control-allow-origin, authorization, content-type")
      .methods("POST"_method, "GET"_method, "PUT"_method, "DELETE"_method, "PATCH"_method, "OPTIONS"_method)
      .allow_credentials();

  CROW_ROUTE(app, "/post")
      .methods("POST"_method)([&](const crow::request& req) {
        // 构建 Crow 响应对象
        // crow::response response(buffer.GetString());
        crow::response response(json_str);
        // 设置响应头部为 JSON 格式
        // response.add_header("Content-Type", "application/json");
        return response;
      });

  // axios 默认不设置发送头的话，默认发送 json 格式的参数数据
  // crow post 默认解析 form 格式的参数数据，对于 json 格式的参数会提示 204 错误
  CROW_ROUTE(app, "/database")
      .methods("POST"_method)([](crow::request req) {
        auto body{req.get_body_params()};
        CROW_LOG_INFO << body;
        return crow::response(200);
      });

  app.bindaddr("0.0.0.0").port(8888).multithreaded().run();
}
