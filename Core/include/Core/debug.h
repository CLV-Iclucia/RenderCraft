//
// Created by creeper on 9/20/23.
//

#ifndef RENDERCRAFT_CORE_INCLUDE_CORE_LOG_H_
#define RENDERCRAFT_CORE_INCLUDE_CORE_LOG_H_
#include <iostream>

namespace rdcraft {

#define ERROR(msg) do { std::cerr << "Error at " << __FILE_NAME__ << ": line " << __LINE__ << ": " << (msg) << std::endl; exit(-1);} while(0)
#define ASSERT(cond, msg) do { if (!(cond)) ERROR(msg); } while(0)
#define CHECK_LT(a, b) ASSERT((a) < (b), "condition not satisfied")
#define CHECK_GT(a, b) ASSERT((a) > (b), "condition not satisfied")
#define CHECK_LE(a, b) ASSERT((a) <= (b), "condition not satisfied")
#define CHECK_GE(a, b) ASSERT((a) >= (b), "condition not satisfied")
#define CHECK_EQ(a, b) ASSERT((a) == (b), "condition not satisfied")
#define CHECK_NE(a, b) ASSERT((a) != (b), "condition not satisfied")
#define CHECK_EQZ(a) ASSERT((a) == (0), "condition not satisfied")
#define CHECK_NEZ(a) ASSERT((a) != (0), "condition not satisfied")

inline void do_nothing() {}

}
#endif //RENDERCRAFT_CORE_INCLUDE_CORE_LOG_H_