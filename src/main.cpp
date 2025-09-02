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
			kdb_print_result(result);
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
				std::string cred = user + ':' + password;
				_kdb_hnd = khpu(const_cast<char*>(host.data()), port, const_cast<char*>(cred.data()));
			} else {
				_kdb_hnd = khpu(const_cast<char*>(host.data()), port, const_cast<char*>(user.data()));
			}
		} else {
			_kdb_hnd = khp(const_cast<char*>(host.data()), port);
		}

		const bool result = kdb_handler_check(_kdb_hnd);
		if(result) {
			printf("connected. _kdb_hnd=%d\n", _kdb_hnd);
		} else {
			printf(" connection failed with code=%d (%s)\n", _kdb_hnd, kdb_handler_error_name(_kdb_hnd));
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

	static const char* kdb_handler_error_name(const int hnd) {
		const char* result = "success";
		if(hnd <= 0) {
			switch (hnd) {
				case 0: return "authentication_failed";
				case -1: return "network error";
				case -2: return "timeout";
				default: return "unknown error";
			}
		}
		return result;
	}

	static bool kdb_handler_check(const int hnd) {
		return hnd > 0;
	}

	static bool kdb_print_result(K result) {
		printf("Result type : %s, ", Kdb::result_name(result->t));
		switch (result->t) {
			case -KB: printf("result (boolean): %s\n", result->g ? "true" : "false"); break;
			case -KG: printf("result (byte): %u\n", result->g); break;
			case -KH: printf("result (short): %hd\n", result->h); break;
			case -KI: printf("result (int): %d\n", result->i); break;
			case -KJ: printf("result (long): %lld\n", result->j); break;
			case -KE: printf("result (real): %g\n", result->e); break;
			case -KF: printf("result (float): %f\n", result->f); break;
			case -KC: printf("result (char): '%c'\n", result->u); break;
			case -KS: printf("result (symbol): %s\n", result->s); break;
			case -KP: printf("result (timestamp): %lld\n", result->j); break;
			case -KM: printf("result (month): %d\n", result->i); break;
			case -KD: printf("result (date): %d\n", result->i); break;
			case -KZ: printf("result (datetime): %f\n", result->f); break;
			case -KN: printf("result (timespan): %lld\n", result->j); break;
			case -KU: printf("result (minute): %d\n", result->i); break;
			case -KV: printf("result (second): %d\n", result->i); break;
			case -KT: printf("result (time): %d\n", result->i); break;

			case KG: printf("result (byte vector): [%lld bytes]\n", result->n); break;  // 4
			case KH: printf("result (short vector): [%lld shorts]\n", result->n); break; // 5
			case KI: printf("result (int vector): [%lld ints]\n", result->n); break;     // 6
			case KJ: printf("result (long vector): [%lld longs]\n", result->n); break;   // 7
			case KE: printf("result (real vector): [%lld reals]\n", result->n); break;   // 8
			case KF: printf("result (float vector): [%lld floats]\n", result->n); break; // 9
			case KC: printf("result (char vector/string): \"%.*s\"\n", (int)result->n, result->G0); break; // 10
			case KS:
				printf("result (symbol vector): [%lld symbols] ", result->n);
				printf("[");
				for(int i = 0; i < result->n; i++) {
					printf("`%s%s", kS(result)[i], i < result->n-1 ? " " : "");
				}
				printf("]\n");

				break; // 11
			case XT: printf("result (table): [table with %lld rows]\n", result->n); break;
			case 0: printf("result (mixed list): [list with %lld items]\n", result->n); break;
			case -128: printf("Error message='%s'\n", result->s); break;
			default:
				printf("Unsupported type: %d\n", result->t);
				return false;
		}
		return true;
	}

	K kdb_send_command() {
		auto command = _cli.command.value().data();
		return k(_kdb_hnd, const_cast<char*>(command), (K)nullptr);
	}

	K kdb_send_trade_data() {
		K row_data = Kdb::create_list(1, Kdb::create_symbol("hello3"));
		return Kdb::execute_sync(_kdb_hnd, "insert", Kdb::create_symbol("trade"), row_data);
	}

	K kdb_send_quote() {
		K row = Kdb::create_list(8,
             Kdb::create_timestamp_from_utc_usec(utc_usec_now()), //  time                 | p
             Kdb::create_symbol(_cli.symbol.value().c_str()),     //  sym                  | s
             Kdb::create_timestamp_from_utc_usec(utc_usec_now()), //  tardisTime           | p
             Kdb::create_long(_cli.id),                           //  orderBookSnapshotId  | j
             Kdb::create_float(_cli.price),                       //  bid                  | f
             Kdb::create_float(_cli.quantity),                    //  bsize                | f
             Kdb::create_float(_cli.price),                       //  ask                  | f
             Kdb::create_float(_cli.quantity)                     //  asize                | f
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
