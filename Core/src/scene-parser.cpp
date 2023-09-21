//
// Created by creeper on 9/21/23.
//
#include <Core/scene-parser.h>
#include <iostream>
namespace rdcraft {

void loadFile(const char* file_path) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(file_path);
  // TODO: implement log lib to replace this
  std::cout << file_path << " parsing result: " << result.description() << std::endl;

}
}