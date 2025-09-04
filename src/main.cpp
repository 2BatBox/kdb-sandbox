#include "KdbSandboxCli.h"
#include "Kdb.h"

#include <chrono>

class KdbSandboxApp {

	const KdbSandboxCli _cli;
	int _kdb_hnd;

public:
	KdbSandboxApp(const KdbSandboxCli& cli) : _cli(cli), _kdb_hnd(-1) {}

	~KdbSandboxApp() {
		kdb_disconnect();
	}

	int run() {
		if(not kdb_connect()) {
			return EXIT_FAILURE;
		}

		K result = nullptr;

		switch (_cli.action.action().value) {
			case KdbSandboxCli::TEST: break;
			case KdbSandboxCli::SEND_CMD: result = kdb_send_command(); break;
			case KdbSandboxCli::SEND_MTRADE: result = kdb_send_trade_data(); break;
			case KdbSandboxCli::SEND_QUOTE: result = kdb_send_quote(); break;

			default:
				fprintf(stderr, "Method %s is not supported.\n", _cli.action.action().to_cstr());
				break;
		}

		if (result == nullptr) {
			printf("No result was given!\n");
		} else {
			Kdb::dump(stdout, result);
			r0(result);
		}

		kdb_disconnect();

		return result ? EXIT_SUCCESS : EXIT_FAILURE;
	}

private:

	static int64_t utc_usec_now() {
		const auto ts_now = std::chrono::system_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::microseconds>(ts_now).count();
	}

	int kdb_connect() {
		const auto host = _cli.host.value();
		const auto port = _cli.port.value();
		printf("Connecting to %s:%u ...", host.c_str(), port);

		// Due to wrong const S definition in KDB header files we have to cast all the "const char*" pointers to "char*".
		// khpu demands "char* const" instead of "const char*".
		if(_cli.user.presented()) {
			const auto user = _cli.user.value();
			if(_cli.password.presented()) {
				const auto password = _cli.password.value();
				_kdb_hnd = Kdb::connect(host.c_str(), port, user.c_str(), password.c_str());
			} else {
				_kdb_hnd = Kdb::connect(host.c_str(), port, user.c_str());
			}
		} else {
			_kdb_hnd = Kdb::connect(host.c_str(), port);
		}

		const bool result = Kdb::handler_check(_kdb_hnd);
		if(result) {
			printf("connected. _kdb_hnd=%d\n", _kdb_hnd);
		} else {
			printf(" connection failed with code=%d (%s)\n", _kdb_hnd, Kdb::handler_error_name(_kdb_hnd));
		}

		return result;
	}

	void kdb_disconnect() {
		if(_kdb_hnd >= 0) {
			kclose(_kdb_hnd);
			printf("Disconnected. _kdb_hnd=%d\n", _kdb_hnd);
			_kdb_hnd = -1;
		}
	}

	K kdb_send_command() {
		auto command = _cli.command.value().data();
		return k(_kdb_hnd, const_cast<char*>(command), (K)nullptr);
	}

	K kdb_send_trade_data() {
		const bool is_buy = _cli.side.value() == KdbSandboxCli::EnumSide::BUY;
		K row = Kdb::create_list(12,
            Kdb::create_timestamp_from_utc_usec(utc_usec_now()),     // time
            Kdb::create_symbol(_cli.symbol.value().c_str()),         // sym
            Kdb::create_symbol(_cli.exchange.value().c_str()),       // exch
            Kdb::create_short(is_buy ? 1 : 0),                       // side
            Kdb::create_float(_cli.price),                           // px
            Kdb::create_float(_cli.quantity),                        // qty
            Kdb::create_long(_cli.id),                               // tradeid
            Kdb::create_timestamp_from_utc_usec(utc_usec_now()),     // exchts1
            Kdb::create_timestamp_from_utc_usec(utc_usec_now()),     // exchts2
            Kdb::create_timestamp_from_utc_usec(utc_usec_now()),     // locts1
            Kdb::create_timestamp_from_utc_usec(utc_usec_now()),     // locts2
            Kdb::create_timestamp_from_utc_usec(utc_usec_now())      // locts3
		);
		auto cmd = _cli.use_insert.presented() ? "insert" : ".u.upd";
		return Kdb::execute_sync(_kdb_hnd, cmd, Kdb::create_symbol("mtrade"), row);
	}

	K kdb_send_quote() {
		K row = Kdb::create_list(9,
             Kdb::create_timestamp_from_utc_usec(utc_usec_now()), //  time                 | p
             Kdb::create_symbol(_cli.symbol.value().c_str()),     //  sym                  | s
             Kdb::create_timestamp_from_utc_usec(utc_usec_now()), //  tardisTime           | p
             Kdb::create_timestamp_from_utc_usec(utc_usec_now()), //  exchangeTime         | p
             Kdb::create_float(_cli.price),                       //  bid                  | f
             Kdb::create_float(_cli.quantity),                    //  bsize                | f
             Kdb::create_float(_cli.price),                       //  ask                  | f
             Kdb::create_float(_cli.quantity),                    //  asize                | f
             Kdb::create_long(_cli.id)                            //  orderBookSnapshotId  | j
		);
		auto cmd = _cli.use_insert.presented() ? "insert" : ".u.upd";
		return Kdb::execute_sync(_kdb_hnd, cmd, Kdb::create_symbol("quote"), row);
	}


};


int main(int argc, char** argv) {
	KdbSandboxCli cli;
	if(not cli.parse_args(argc, argv)) {
		cli.print_usage(stderr, argv[0]);
		return EXIT_FAILURE;
	}
	KdbSandboxApp app(cli);
	return app.run();
}
