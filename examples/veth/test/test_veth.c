#include "unity.h"
#include "veth.c" /* for testing static functions */

#include "mock_veth_dep.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test__veth_init__call_rtnl_link_register_return_expected_value(void)
{
				int ret;
				const int return_value = 10;

				rtnl_link_register_IgnoreAndReturn(return_value);
				ret = veth_init();
				TEST_ASSERT_EQUAL(return_value, ret);
}

static int cmock_rtnl_link_register_callback(struct rtnl_link_ops* ops, int cmock_num_calls)
{
				TEST_ASSERT(cmock_num_calls == 0); // call once

				TEST_ASSERT_EQUAL_STRING(DRV_NAME, ops->kind);
				TEST_ASSERT_EQUAL(sizeof(struct veth_priv), ops->priv_size);
				TEST_ASSERT_EQUAL_PTR((void*)veth_setup, ops->setup);
				TEST_ASSERT_EQUAL_PTR((void*)veth_validate, ops->validate);
				TEST_ASSERT_EQUAL_PTR((void*)veth_newlink, ops->newlink);
				TEST_ASSERT_EQUAL_PTR((void*)veth_dellink, ops->dellink);
				TEST_ASSERT_EQUAL_PTR( (void*)veth_policy, ops->policy);
				TEST_ASSERT_EQUAL(VETH_INFO_MAX, ops->maxtype);
				return 0;
}

void test__veth_init__call_rtnl_link_register_with_expected_argument(void)
{
				rtnl_link_register_StubWithCallback(cmock_rtnl_link_register_callback);
				veth_init();
}
