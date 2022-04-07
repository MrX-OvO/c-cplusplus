#include <fstream>
#include <iostream>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>

/*
   [
   "xiaoming", 19, 170, false,
   ["zhangsan", "lisi"],
   {"age":30, "gender":"man"}
   ]
   */

void writeJson(bool style = true) {
  Json::Value root;
  root.append("xiaoming");
  root.append(19);
  root.append(170);
  root.append(false);

  Json::Value subArray;
  subArray.append("zhangsan");
  subArray.append("lisi");
  root.append(subArray);

  Json::Value obj;
  obj["age"] = 30;
  obj["gender"] = "man";
  root.append(obj);

  std::string json;
  if (style) {
    json = root.toStyledString();
  } else {
    Json::FastWriter w;
    json = w.write(root);
  }
  std::ofstream ofs("./test.json");
  ofs << json;
  ofs.close();
}

void readJson(const char *path) {
  std::ifstream ifs(path);
  Json::Reader r;
  Json::Value root;
  r.parse(ifs, root);
}

int main() {
  bool style = true;
  const char *path = "./test.json";

  writeJson();
  readJson(path);

  return 0;
}
