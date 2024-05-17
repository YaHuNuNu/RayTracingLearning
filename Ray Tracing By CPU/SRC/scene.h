#pragma once

#include <vector>
#include "model.h"

//存储模型的变量，发光物一定为三角形且存在最前面
extern std::vector<model*> models;
//加载模型
void loadScene();
//清理模型
void clearScene();