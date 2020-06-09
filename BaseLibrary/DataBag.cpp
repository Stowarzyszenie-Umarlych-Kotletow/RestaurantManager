#include "DataBag.h"

void DataBag::clear() {
	_map.clear();
}

void DataBag::remove(std::string key) {
	_map.erase(key);
}
