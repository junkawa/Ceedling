#ifndef _VETH_DEP_H_
#define _VETH_DEP_H_

#include <linux/netdevice.h>
#include <linux/slab.h>
#include <linux/ethtool.h>
#include <linux/etherdevice.h>
#include <linux/u64_stats_sync.h>

#include <net/dst.h>
#include <net/xfrm.h>
#include <linux/veth.h>
#include <linux/module.h>

void __percpu *__alloc_percpu(size_t size, size_t align);
int dev_forward_skb(struct net_device *dev, struct sk_buff *skb);
void ether_setup(struct net_device *dev);
u32 ethtool_op_get_link(struct net_device *dev);
void free_netdev(struct net_device *dev);
void free_percpu(void __percpu *ptr);
void kfree_skb(struct sk_buff *skb);
void netif_carrier_off(struct net_device *dev);
void netif_carrier_on(struct net_device *dev);
int nla_parse(struct nlattr **tb, int maxtype, const struct nlattr *head,
											int len, const struct nla_policy *policy);
size_t nla_strlcpy(char *dst, const struct nlattr *nla, size_t dstsize);
int register_netdevice(struct net_device *dev);
int rtnl_configure_link(struct net_device *dev, const struct ifinfomsg *ifm);
struct net_device *rtnl_create_link(struct net *net,
																						char *ifname, const struct rtnl_link_ops *ops, struct nlattr *tb[]);
struct net *rtnl_link_get_net(struct net *src_net, struct nlattr *tb[]);
int rtnl_link_register(struct rtnl_link_ops *ops);
void rtnl_link_unregister(struct rtnl_link_ops *ops);
void unregister_netdevice_queue(struct net_device *dev, struct list_head *head);

#endif /* _VETH_DEP_H_ */
