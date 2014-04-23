#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>

#define DIR_NAME "unittest"

/* total output size LOG_FILE_NUM*LOG_FILE_SIZE */
#define LOG_FILE_NUM  (10)
#define LOG_FILE_SIZE (4000)

static struct kobject *unittest_kobj;

static char buffer[LOG_FILE_NUM][LOG_FILE_SIZE];
static int buffer_idx[LOG_FILE_NUM];

int kernel_module_putchar(int a)
{
				int i;
				int idx = 0;
				for (i=0; i<LOG_FILE_NUM; i++) {
								if (buffer_idx[i]<LOG_FILE_SIZE) break;
				}
				if (i == LOG_FILE_NUM) return 0;
				idx = i;

				buffer[idx][buffer_idx[idx]] = (char)a;
				buffer_idx[idx] += 1;

				return 0;
}

static ssize_t unittest_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
				int ret;
				int idx = attr->attr.name[0]-'0';
				int size = buffer_idx[idx]+1;
				if (idx>=LOG_FILE_NUM) return 0;

				ret = snprintf(buf, size, "%s", buffer[idx]);
				if (ret >= 0) {
								int len = (ret < size) ? ret : size - 1;
								buf[len] = '\0';
				}
				return ret;
}

static struct kobj_attribute unittest_attr0 = __ATTR(0, 0444, unittest_show, NULL);
static struct kobj_attribute unittest_attr1 = __ATTR(1, 0444, unittest_show, NULL);
static struct kobj_attribute unittest_attr2 = __ATTR(2, 0444, unittest_show, NULL);
static struct kobj_attribute unittest_attr3 = __ATTR(3, 0444, unittest_show, NULL);
static struct kobj_attribute unittest_attr4 = __ATTR(4, 0444, unittest_show, NULL);
static struct kobj_attribute unittest_attr5 = __ATTR(5, 0444, unittest_show, NULL);
static struct kobj_attribute unittest_attr6 = __ATTR(6, 0444, unittest_show, NULL);
static struct kobj_attribute unittest_attr7 = __ATTR(7, 0444, unittest_show, NULL);
static struct kobj_attribute unittest_attr8 = __ATTR(8, 0444, unittest_show, NULL);
static struct kobj_attribute unittest_attr9 = __ATTR(9, 0444, unittest_show, NULL);

static struct attribute *unittest_attrs[] = {
				&unittest_attr0.attr,
				&unittest_attr1.attr,
				&unittest_attr2.attr,
				&unittest_attr3.attr,
				&unittest_attr4.attr,
				&unittest_attr5.attr,
				&unittest_attr6.attr,
				&unittest_attr7.attr,
				&unittest_attr8.attr,
				&unittest_attr9.attr,
				NULL,
};

static struct attribute_group unittest_attr_group = {
				.attrs = unittest_attrs,
};

static int sysffs_init(void)
{
				int retval;

				unittest_kobj = kobject_create_and_add(DIR_NAME, kernel_kobj);
				if(!unittest_kobj){
								return -ENOMEM;
				}

				retval = sysfs_create_group(unittest_kobj, &unittest_attr_group);
				if(retval){
								kobject_put(unittest_kobj);
				}

				return retval;
}

extern int main(void);
static int kernel_module_init(void)
{
				int ret;
				ret = sysffs_init();
				if (ret) return ret;

				main(); // test runner main function.

				return 0;
}

static void kernel_module_exit(void)
{
				kobject_put(unittest_kobj);
}

module_init(kernel_module_init);
module_exit(kernel_module_exit);
MODULE_LICENSE("GPL");
