#pragma once
#include <map>
#include <list>
#include <string.h>
#include <algorithm>
#include "disposable.h"
using namespace std;
namespace vocaloid {	

	typedef void(*callback)(void*);

	class Emitter : public IDisposable {

	private:
		map<string, list<callback>> _handlers;

	public:

		Emitter() {}

		bool Contains(const string name, callback cb) {
			auto iter1 = _handlers.find(name);
			if (iter1 == _handlers.end()) {
				return false;
			}
			auto iter2 = std::find(iter1->second.begin(), iter1->second.end(), cb);
			if (iter2 != iter1->second.end())return true;
			return false;
		}

		int On(const string event, callback handler) {
			auto iter1 = _handlers.find(event);
			list<callback> callback_list;
			if (iter1 != _handlers.end()) {
				callback_list = iter1->second;
			}
			else {
				_handlers[event] = callback_list;
			}
			_handlers[event].push_back(handler);
			return 0;
		}

		void Emit(const string event, void* data) {
			auto iter1 = _handlers.find(event);
			if (iter1 == _handlers.end())return;
			for (auto handler : iter1->second) {
				handler(data);
			}
		}

		void off(const string event, callback cb) {
			auto iter1 = _handlers.find(event);
			if (iter1 != _handlers.end()) {
				auto iter2 = find(iter1->second.begin(), iter1->second.end(), cb);
				if (iter2 != iter1->second.end()) {
					iter1->second.erase(iter2);
				}
			}
		}

		void Dispose() override {
			_handlers.clear();
		}
	};
}