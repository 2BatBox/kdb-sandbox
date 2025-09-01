#pragma once

#include "AppCli.h"

#include <cstdint>
#include <string>

struct KdbSandboxCli {

	enum EnumMethod : unsigned {
		TEST,
		SEND_CMD,
		SEND_TRADE,
		__SIZE
	};

	struct EnumMethodToCStr {
		static const char* to_cstr(const EnumMethod& value) {
			switch(value) {
				case EnumMethod::TEST: return "test";
				case EnumMethod::SEND_CMD: return "cmd";
				case EnumMethod::SEND_TRADE: return "trade";
				default: return "[UNKNOWN]";
			}
		}
	};

	using Method = EnumField<EnumMethod, EnumMethodToCStr>;

	enum class EnumSide : unsigned {
		BUY,
		SELL,
		__SIZE
	};

	struct EnumSideToCStr {
		static const char* to_cstr(const EnumSide& value) {
			switch(value) {
				case EnumSide::BUY: return "buy";
				case EnumSide::SELL: return "sell";
				default: return "[UNKNOWN]";
			}
		}
	};

	using Side = EnumField<EnumSide, EnumSideToCStr>;

	unsigned pr = 1;
	Option<std::string> host = Option<std::string>('h', "KDB host IP.", ++pr);
	Option<int> port = Option<int>('p', "KDB port.", ++pr);
	Option<std::string> user = Option<std::string>('u', "KDB user name.", ++pr);
	Option<std::string> password = Option<std::string>('s', "KDB user password.", ++pr);
	Option<std::string> command = Option<std::string>('c', "User defined KDB command.", ++pr);

	Option<unsigned> rounds = Option<unsigned>('r', "How many", ++pr);

	Option<std::string> exchange = Option<std::string>('E', "Exchange name.", ++pr);
	Option<long> timestamp = Option<long>('T', "Timestamp in nanoseconds.", ++pr);
	Option<double> price = Option<double>('P', "Price.", ++pr);
	Option<double> quantity = Option<double>('Q', "Quantity.", ++pr);
	Option<Side> side = Option<Side>('S', Side::description(), ++pr);
	Option<std::string> id = Option<std::string>('I', "ID.", ++pr);

//	OptionFlag stay_connected = OptionFlag('j', "Show kanji before.", ++pr);

	AppCliMethod<Method> action;

	KdbSandboxCli() {
		action[EnumMethod::TEST].desc("Test connection.").mand(host, port).opt(user, password);
		action[EnumMethod::SEND_CMD].desc("Send user command.").mand(host, port, command).opt(user, password);
		action[EnumMethod::SEND_TRADE].desc("Send order data.").mand(host, port, exchange, timestamp, price, quantity, side, id).opt(user, password);
		action.finalize();
	}

	bool parse_args(int argc, char** argv) {
		return action.parse_args(argc, argv) && validate();
	}

	bool validate() const {
		bool result = true;
//		result = result && host.value()empty();
		return result;
	}

	void print_usage(FILE* out, const char* bin) {
		action.print_usage(out, bin);
	}

	std::string options_string() const {
		return action.options_string();
	}

};
