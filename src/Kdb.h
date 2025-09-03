#pragma once

#include <k.h>
#include <string>

struct Kdb {

	static void dump(FILE* out, const K& k) {
		fprintf(out, "k0 %p", k);
		fprintf(out, " memory_type=%d(%s)", k->m, memory_name(k->m));
		fprintf(out, " attribute=%d(%s)", k->a, attrib_name(k->a));
		fprintf(out, " type=%d(%s)", k->t, type_name(k->t));
		fprintf(out, " ptr_counter=%d", k->r);
		fprintf(out, "\n");
		fprintf(out, " Payload (%s) : ", type_name(k->t));
		dump_data(out, k);
		fprintf(out, "\n");
	}

	static void dump_data(FILE* out, const K& k) {
		if(k->t >= 0) {
			fprintf(out, "%lld items [", k->n);
		}
		switch (k->t) {
			case -KB: fprintf(out, "%s", k->g ? "true" : "false"); break;
			case -KG: fprintf(out, "%u", k->g); break;
			case -KH: fprintf(out, "%hd", k->h); break;
			case -KI: fprintf(out, "%d", k->i); break;
			case -KJ: fprintf(out, "%lld", k->j); break;
			case -KE: fprintf(out, "%g", k->e); break;
			case -KF: fprintf(out, "%f", k->f); break;
			case -KC: fprintf(out, "'%c'", k->g); break;
			case -KS: fprintf(out, "%s", k->s); break;
			case -KP: fprintf(out, "%lld", k->j); break;
			case -KM: fprintf(out, "%d", k->i); break;
			case -KD: fprintf(out, "%d", k->i); break;
			case -KZ: fprintf(out, "%f", k->f); break;
			case -KN: fprintf(out, "%lld", k->j); break;
			case -KU: fprintf(out, "%d", k->i); break;
			case -KV: fprintf(out, "%d", k->i); break;
			case -KT: fprintf(out, "%d", k->i); break;

			case KG: for(int i = 0; i < k->n; i++) { fprintf(out, "%c%s",   kG(k)[i], i < k->n-1 ? ", " : ""); } break;
			case KH: for(int i = 0; i < k->n; i++) { fprintf(out, "%d%s",   kH(k)[i], i < k->n-1 ? ", " : ""); } break;
			case KI: for(int i = 0; i < k->n; i++) { fprintf(out, "%d%s",   kI(k)[i], i < k->n-1 ? ", " : ""); } break;
			case KJ: for(int i = 0; i < k->n; i++) { fprintf(out, "%lld%s", kJ(k)[i], i < k->n-1 ? ", " : ""); } break;
			case KE: for(int i = 0; i < k->n; i++) { fprintf(out, "%f%s",   kE(k)[i], i < k->n-1 ? ", " : ""); } break;
			case KF: for(int i = 0; i < k->n; i++) { fprintf(out, "%f%s",   kF(k)[i], i < k->n-1 ? ", " : ""); } break;
			case KC: fprintf(out, "'%.*s'", (int)k->n, k->G0); break; // 10
			case KS: for(int i = 0; i < k->n; i++) { printf("%s%s", kS(k)[i], i < k->n-1 ? ", " : ""); } break;

//			case XT: fprintf(out, "(table): [table with %lld rows]", k->n); break;
			case 0:
			case XT:
			case XD:
			{
				K* elements = (K*) k->G0;
				for (int i = 0; i < k->n; i++) {
					dump(out, elements[i]);
				}
			}
			break;

			case -128: fprintf(out, "Error message='%s'", k->s); break;
			default:
				printf("Unsupported type: %d", k->t);
		}
		if(k->t >= 0) {
			fprintf(out, "]");
		}
	}

	static const char* memory_name(const signed char mem) {
		switch (mem) {
			case 0: return "heap";
			case 1: return "stack";
			case 2: return "mapped";
			default : return "[UNKNOWN]";
		}
	}

	static const char* attrib_name(const signed char attr) {
		switch (attr) {
			case 0: return "default";
			case 1: return "sorted";
			case 2: return "unique";
			case 3: return "parted";
			case 4: return "grouped";
			default : return "[UNKNOWN]";
		}
	}

	static const char* type_name(const signed char type) {
		switch (type) {
			case 0: return "mixed list";

			case -KB: return "boolean";
			case -KG: return "byte";
			case -KH: return "short";
			case -KI: return "int";
			case -KJ: return "long";
			case -KE: return "real";
			case -KF: return "float";
			case -KC: return "char";
			case -KS: return "symbol";
			case -KP: return "timestamp";
			case -KM: return "month";
			case -KD: return "date";
			case -KZ: return "datetime";
			case -KN: return "timespan";
			case -KU: return "minute";
			case -KV: return "second";
			case -KT: return "time";

			case KB: return "boolean vector";
			case KG: return "byte vector";
			case KH: return "short vector";
			case KI: return "int vector";
			case KJ: return "long vector";
			case KE: return "real vector";
			case KF: return "float vector";
			case KC: return "char vector";
			case KS: return "symbol vector";
			case KP: return "timestamp vector";
			case KM: return "month vector";
			case KD: return "date vector";
			case KZ: return "datetime vector";
			case KN: return "timespan vector";
			case KU: return "minute vector";
			case KV: return "second vector";
			case KT: return "time vector";
			case XT: return "table";
			case XD: return "dict";

			case 100 : return "lambda/function";
			case 101 : return "unary_primitive";
			case 102 : return "binary_primitive";
			case 103 : return "ternary_primitive";
			case 104 : return "projection";
			case 105 : return "composition";

			case -128: return "error";
			default: return "[UNKNOWN]";
		}
	}

	static bool handler_check(const int hnd) {
		return hnd > 0;
	}

	static const char* handler_error_name(const int hnd) {
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

	static K create_boolean(bool val) { return kb(val); }
	static K create_byte(unsigned char val) { return kg(val); }
	static K create_short(short val) { return kh(val); }
	static K create_int(int val) { return ki(val); }
	static K create_long(long val) { return kj(val); }
	static K create_real(float val) { return ke(val); }
	static K create_float(double val) { return kf(val); }
	static K create_char(char val) { return kc(val); }
	static K create_symbol(const char* val) { return ks(const_cast<char*>(val)); }
	static K create_timestamp(long val) { return ktj(-KP, val); }
	static K create_month(int val) { return ktj(-KM, val); }
	static K create_date(int val) { return ktj(-KD, val); }
	static K create_datetime(double val) { return ktj(-KZ, val); }
	static K create_timespan(long val) { return ktj(-KN, val); }
	static K create_minute(int val) { return ktj(-KU, val); }
	static K create_second(int val) { return ktj(-KV, val); }
	static K create_time(int val) { return ktj(-KT, val); }

	static K create_string(const char* val) { return kpn(const_cast<char*>(val), static_cast<long long>(strlen(val))); }
	static K create_string_n(const char* val, int len) { return kpn(const_cast<char*>(val), len); }

	static K create_list(int count, ...) {
		va_list args;
		va_start(args, count);
		K result = knk(count);
		for (int i = 0; i < count; i++) {
			kK(result)[i] = va_arg(args, K);
		}
		va_end(args);
		return result;
	}

	static K create_timestamp_from_utc_usec(const int64_t usec) {
		return create_timestamp((usec - 946684800000000LL) * 1000LL);
	}

	static K create_timespan_from_utc_usec(const int64_t usec) {
		return create_timespan((usec - 946684800000000LL) * 1000LL);
	}

	static K create_empty_list() { return knk(0); }
	static K create_null() { return (K)nullptr; }

	static int connect(const char* host, int port) {
		return khp(const_cast<char*>(host), port);
	}

	static int connect(const char* host, int port, const char* user) {
		return khpu(const_cast<char*>(host), port, const_cast<char*>(user));
	}

	static int connect(const char* host, int port, const char* user, const char* password) {
		std::string cred(user);
		cred.push_back(':');
		cred.append(password);
		return khpu(const_cast<char*>(host), port, const_cast<char*>(cred.data()));
	}

	static void disconnect(int handle) {
		kclose(handle);
	}

	static K execute_sync(int handle, const char* cmd) {
		return k(handle, const_cast<char*>(cmd), (K)nullptr);
	}

	static K execute_sync(int handle, const char* cmd, K args0) {
		return k(handle, const_cast<char*>(cmd), args0, (K)nullptr);
	}

	static K execute_sync(int handle, const char* cmd, K args0, K args1) {
		return k(handle, const_cast<char*>(cmd), args0, args1, (K)nullptr);
	}

	static void free_object(K obj) {
		if (obj) r0(obj);
	}

	static K ref_object(K obj) {
		return r1(obj);
	}

	static bool is_atom(K obj) { return obj && obj->t < 0; }
	static bool is_vector(K obj) { return obj && obj->t > 0 && obj->t < 20; }
	static bool is_table(K obj) { return obj && obj->t == XT; }
	static bool is_dict(K obj) { return obj && obj->t == XD; }
	static bool is_error(K obj) { return obj && obj->t == -128; }
	static bool is_null(K obj) { return obj == (K)nullptr; }

	static bool get_boolean(K obj) { return obj->g; }
	static unsigned char get_byte(K obj) { return obj->g; }
	static short get_short(K obj) { return obj->h; }
	static int get_int(K obj) { return obj->i; }
	static long get_long(K obj) { return obj->j; }
	static float get_real(K obj) { return obj->e; }
	static double get_float(K obj) { return obj->f; }
	static char get_char(K obj) { return obj->u; }
	static char* get_symbol(K obj) { return obj->s; }
	static char* get_error_msg(K obj) { return obj->s; }
	static long get_count(K obj) { return obj->n; }

	static char* get_string_data(K obj) { return (char*)obj->G0; }
	static char** get_symbol_vector(K obj) { return (char**)obj->G0; }
	static int* get_int_vector(K obj) { return (int*)obj->G0; }
	static long* get_long_vector(K obj) { return (long*)obj->G0; }
	static double* get_float_vector(K obj) { return (double*)obj->G0; }
};
