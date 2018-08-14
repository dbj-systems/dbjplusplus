#pragma once

#include "../dbj_key_value_store.h"

DBJ_TEST_SPACE_OPEN(kv_storage_test)


DBJ_TEST_UNIT( dbj_kv_storage_test ) {

	dbj::sync::lock_unlock padlock_;

	using KVS = dbj::storage::keyvalue_storage<int>;

	KVS kvs{};

	kvs.add(L"K1", 0);
	kvs.add(L"K2", 1);
	kvs.add(L"K3", 2);

	KVS::value_vector  val_ = kvs.retrieve(L"K");

	static INT DBJ_UNUSED(anchor) { 1 };
}

DBJ_TEST_SPACE_CLOSE