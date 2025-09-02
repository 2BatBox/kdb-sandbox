#pragma once

#include <k.h>
#include <string>

struct Kdb {

//	-1   KB   boolean
//	-2   UU   guid
//	-4   KG   byte
//	-5   KH   short
//	-6   KI   int
//	-7   KJ   long
//	-8   KE   real
//	-9   KF   float
//	-10  KC   char
//	-11  KS   symbol
//	-12  KP   timestamp
//	-13  KM   month
//	-14  KD   date
//	-15  KZ   datetime
//	-16  KN   timespan
//	-17  KU   minute
//	-18  KV   second
//	-19  KT   time

//	1    KB   boolean vector
//	2    UU   guid vector
//	4    KG   byte vector
//	5    KH   short vector
//	6    KI   int vector
//	7    KJ   long vector
//	8    KE   real vector
//	9    KF   float vector
//	10   KC   char vector (string)
//	11   KS   symbol vector
//	12   KP   timestamp vector
//	13   KM   month vector
//	14   KD   date vector
//	15   KZ   datetime vector
//	16   KN   timespan vector
//	17   KU   minute vector
//	18   KV   second vector
//	19   KT   time vector

//	0    mixed list
//	98   XT   table
//	99   XD   dictionary
//	100  lambda/function
//	101  unary primitive
//	102  binary primitive
//	103  ternary primitive
//	104  projection
//	105  composition

	static const char* result_name(const signed char type) {
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

			case KG: return "byte vector";
			case KH: return "short vector";
			case KI: return "int vector";
			case KJ: return "long vector";
			case KE: return "real vector";
			case KF: return "float vector";
			case KC: return "char vector/string";
			case KS: return "symbol vector";
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

	static K create_single_timestamp(long value) {
		K result = ktn(KP, 1);
		kJ(result)[0] = value;
		return result;
	}

	static K create_single_symbol(const char* value) {
		K result = ktn(KS, 1);
		kS(result)[0] = ss(const_cast<char*>(value));
		return result;
	}

	static K create_single_long(long value) {
		K result = ktn(KJ, 1);
		kJ(result)[0] = value;
		return result;
	}

	static K create_single_float(double value) {
		K result = ktn(KF, 1);
		kF(result)[0] = value;
		return result;
	}


	static K create_single_timestamp_from_utc_usec(const int64_t usec) {
		return create_single_timestamp((usec - 946684800000000LL) * 1000LL);
	}

	static K create_timestamp_from_utc_usec(const int64_t usec) {
		return create_timestamp((usec - 946684800000000LL) * 1000LL);
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
