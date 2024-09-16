#pragma once
#include "idocumentnode.h"
#include "inodecollection.h"
#include "textnode.h"
#include <memory>
#include <vector>

namespace htcpp{

std::vector<std::unique_ptr<IDocumentNode>> flattenNodes(std::vector<std::unique_ptr<IDocumentNode>> nodes);
std::vector<std::unique_ptr<IDocumentNode>> optimizeNodes(std::vector<std::unique_ptr<IDocumentNode>> nodes);

}