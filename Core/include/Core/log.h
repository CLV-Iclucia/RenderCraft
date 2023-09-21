//
// Created by creeper on 9/20/23.
//

#ifndef RENDERCRAFT_CORE_INCLUDE_CORE_LOG_H_
#define RENDERCRAFT_CORE_INCLUDE_CORE_LOG_H_
#include <iostream>
namespace rdcraft {
#define ERROR(msg) { std::cerr << "Error at " << __FILE_NAME__ << ": line " << __LINE__ << ": " << msg << std::endl; exit(-1);}

}
#endif //RENDERCRAFT_CORE_INCLUDE_CORE_LOG_H_
